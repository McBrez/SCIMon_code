// Standard includes
#include <filesystem>

// 3rd-party includes
#include <easylogging++.h>

// Project includes
#include <data_manager_hdf.hpp>

using namespace Utilities;
using namespace HighFive;
using namespace Core;

std::mutex DataManagerHdf::dataManagerMutex = std::mutex();

DataManagerHdf::~DataManagerHdf() { this->close(); }

bool DataManagerHdf::read(TimePoint timestamp, const std::string &key,
                          Value &value) {
  if (!this->isOpen()) {
    return false;
  }
  if (!this->typeMapping.contains(key)) {
    return false;
  }
  // If the key refers to a spectrum type, the spectrum has to be setup first.
  if (this->typeMapping[key] == DATAMANAGER_DATA_TYPE_SPECTRUM &&
      !this->isSpectrumSetup(key)) {
    return false;
  }

  std::lock_guard<std::mutex> lockGuard(this->dataManagerMutex);

  DataSet datasetTimestamps =
      this->hdfFile->getDataSet("/data/" + key + "/timestamps");

  // Read chunk by chunk and try to find a matching timestamp.
  std::vector<long long> timestampLongRead(this->defaultChunkingSize);
  for (hsize_t i = 0; i < datasetTimestamps.getElementCount();
       i += this->defaultChunkingSize) {
    // Determine the count of elements that shall be read.
    hsize_t elementCount =
        (i + this->defaultChunkingSize) > datasetTimestamps.getElementCount()
            ? datasetTimestamps.getElementCount() - i
            : this->defaultChunkingSize;
    datasetTimestamps.select({i, 0}, {elementCount, 1}).read(timestampLongRead);

    // Iterate over the gathered timestamps and try to find a match.
    for (auto timestampRaw : timestampLongRead) {
      TimePoint timestampRead =
          TimePoint(std::chrono::milliseconds(timestampRaw));
      if (this->timePointDiff(timestampRead, timestamp) <
          std::chrono::milliseconds(1)) {
        DataManagerDataType dataType = this->typeMapping[key];
        if (DATAMANAGER_DATA_TYPE_INT == dataType) {
          DataSet datasetValues =
              this->hdfFile->getDataSet("/data/" + key + "/values");
          value = datasetValues.select({i, 0}, {1, 1}).read<int>();

          return true;
        } else if (DATAMANAGER_DATA_TYPE_DOUBLE == dataType) {
          DataSet datasetValues =
              this->hdfFile->getDataSet("/data/" + key + "/values");
          value = datasetValues.select({i, 0}, {1, 1}).read<double>();

          return true;
        } else if (DATAMANAGER_DATA_TYPE_COMPLEX == dataType) {
          DataSet datasetValues =
              this->hdfFile->getDataSet("/data/" + key + "/values");
          Impedance impedance(
              datasetValues.select({i, 0}, {1, 1}).read<double>(),
              datasetValues.select({i, 1}, {1, 1}).read<double>());
          value = impedance;

          return true;

        } else if (DATAMANAGER_DATA_TYPE_STRING == dataType) {
          DataSet datasetValues =
              this->hdfFile->getDataSet("/data/" + key + "/values");
          value = datasetValues.select({i, 0}, {1, 1}).read<std::string>();

          return true;

        } else if (DATAMANAGER_DATA_TYPE_SPECTRUM == dataType) {
          DataSet datasetValues =
              this->hdfFile->getDataSet("/data/" + key + "/values");
          size_t frequencyCount = this->spectrumMapping[key].size();
          auto spectrumArray =
              datasetValues.select({i, 0, 0}, {1, frequencyCount, 2})
                  .read<std::vector<std::vector<std::vector<double>>>>();
          std::vector<ImpedanceSpectrum> isSpectrum;
          Utilities::joinImpedanceSpectrum(
              spectrumArray, this->spectrumMapping[key], isSpectrum);

          value = isSpectrum.front();

          return true;
        }

        else {
          return false;
        }
      }
    }
  }

  return false;
}

bool DataManagerHdf::read(TimePoint from, TimePoint to, const std::string &key,
                          std::vector<TimePoint> &timestamps,
                          std::vector<Value> &value) {

  if (!this->isOpen()) {
    return false;
  }
  if (!this->typeMapping.contains(key)) {
    return false;
  }
  if (from > to) {
    return false;
  }

  std::lock_guard<std::mutex> lockGuard(this->dataManagerMutex);

  DataSet datasetTimestamps =
      this->hdfFile->getDataSet("/data/" + key + "/timestamps");

  // Iterate chunk by chunk over the whole dataset and find the index margins of
  // the requested time frame.
  hsize_t idxFrom = 0;
  hsize_t idxTo = 0;
  bool idxFromFound = false;
  bool idxToFound = false;
  std::vector<long long> timestampLongRead;
  timestampLongRead.reserve(this->defaultChunkingSize);
  for (hsize_t i = 0; i < datasetTimestamps.getElementCount();
       i += this->defaultChunkingSize) {
    // Determine the count of elements that shall be read. Ensure that it is not
    // read beyond the bounds of the dataset.
    hsize_t elementCount =
        (i + this->defaultChunkingSize) > datasetTimestamps.getElementCount()
            ? datasetTimestamps.getElementCount() - i
            : this->defaultChunkingSize;
    datasetTimestamps.select({i, 0}, {elementCount, 1}).read(timestampLongRead);

    hsize_t offset = 0;
    for (auto timestampRaw : timestampLongRead) {
      TimePoint timestampCurrent((std::chrono::milliseconds(timestampRaw)));
      if (timestampCurrent >= from && timestampCurrent <= to && !idxFromFound) {
        idxFrom = i + offset;
        idxFromFound = true;
      }
      if (timestampCurrent >= to && !idxToFound) {
        idxTo = i + offset;
        idxToFound = true;
        // The fact that idxTo has been found implies that idxFrom has been
        // found too, as it is assumed that timestamps are ordered. Break the
        // loop here, as both indices have been found.
        break;
      }
      offset++;
    }

    // Break the loop, if both indices have been found.
    if (idxFromFound && idxToFound) {
      break;
    }
  }

  // Check if the indices have been found. idxFrom has to be greater or equal 0.
  // idxTo optionally needs to be greater or equal 0. An idxTo of -1 means, that
  // the idx would lie beyond the range of the dataset. In that case, just take
  // the last element as idxTo.
  if (!idxFromFound) {
    // Return empty vector.
    value = std::vector<Value>();

    return true;
  }
  if (!idxToFound) {
    idxTo = datasetTimestamps.getElementCount() - 1;
  }

  // Get the timestamps and construct a std::vector<TimePoints>.
  std::vector<long long> timestampsRaw;
  datasetTimestamps.select({idxFrom, 0}, {idxTo - idxFrom + 1, 1})
      .read(timestampsRaw);
  timestamps.reserve(timestampsRaw.size());
  for (auto timestampsRawValue : timestampsRaw) {
    timestamps.emplace_back(
        TimePoint(std::chrono::milliseconds(timestampsRawValue)));
  }

  // Read from the data set and construct a std::vector<Value>.
  DataManagerDataType dataType = this->typeMapping[key];
  if (DATAMANAGER_DATA_TYPE_INT == dataType) {
    DataSet datasetValues =
        this->hdfFile->getDataSet("/data/" + key + "/values");
    std::vector<int> rawVector =
        datasetValues.select({idxFrom, 0}, {idxTo - idxFrom + 1, 1})
            .read<std::vector<int>>();
    value.reserve(rawVector.size());
    for (auto rawVectorValue : rawVector) {
      value.emplace_back(Value(rawVectorValue));
    }

    return true;
  }

  else if (DATAMANAGER_DATA_TYPE_DOUBLE == dataType) {
    DataSet datasetValues =
        this->hdfFile->getDataSet("/data/" + key + "/values");
    std::vector<double> rawVector =
        datasetValues.select({idxFrom, 0}, {idxTo - idxFrom + 1, 1})
            .read<std::vector<double>>();
    value.reserve(rawVector.size());
    for (auto rawVectorValue : rawVector) {
      value.emplace_back(Value(rawVectorValue));
    }

    return true;
  }

  else if (DATAMANAGER_DATA_TYPE_COMPLEX == dataType) {
    DataSet datasetValues =
        this->hdfFile->getDataSet("/data/" + key + "/values");
    std::vector<double> rawRealValues =
        datasetValues.select({idxFrom, 0}, {idxTo - idxFrom + 1, 1})
            .read<std::vector<double>>();
    std::vector<double> rawImagValues =
        datasetValues.select({idxFrom, 1}, {idxTo - idxFrom + 1, 1})
            .read<std::vector<double>>();

    value.reserve(rawRealValues.size());
    for (int i = 0; i < rawRealValues.size(); i++) {
      value.emplace_back(Impedance(rawRealValues[i], rawImagValues[i]));
    }

    return true;
  }

  else if (DATAMANAGER_DATA_TYPE_STRING == dataType) {
    DataSet datasetValues =
        this->hdfFile->getDataSet("/data/" + key + "/values");
    std::vector<std::string> rawVector =
        datasetValues.select({idxFrom, 0}, {idxTo - idxFrom + 1, 1})
            .read<std::vector<std::string>>();
    value.reserve(rawVector.size());
    for (auto rawVectorValue : rawVector) {
      value.emplace_back(Value(rawVectorValue));
    }

    return true;
  }

  else if (DATAMANAGER_DATA_TYPE_SPECTRUM == dataType) {
    DataSet datasetValues =
        this->hdfFile->getDataSet("/data/" + key + "/values");
    size_t frequencyCount = this->spectrumMapping[key].size();
    auto spectrumArray =
        datasetValues
            .select({idxFrom, 0, 0}, {idxTo - idxFrom + 1, frequencyCount, 2})
            .read<std::vector<std::vector<std::vector<double>>>>();
    std::vector<ImpedanceSpectrum> isSpectrum;
    Utilities::joinImpedanceSpectrum(spectrumArray, this->spectrumMapping[key],
                                     isSpectrum);

    value.reserve(isSpectrum.size());
    for (auto &rawVectorValue : isSpectrum) {
      value.emplace_back(Value(rawVectorValue));
    }

    return true;
  }

  else {
    return false;
  }
}

bool DataManagerHdf::write(TimePoint timestamp, const std::string &key,
                           const Value &value) {

  if (!this->isOpen()) {
    return false;
  }

  if (!this->typeMapping.contains(key)) {
    return false;
  }

  return this->extendingWrite({timestamp}, key, {value});
}

bool DataManagerHdf::write(const std::vector<TimePoint> &timestamp,
                           const std::string &key,
                           const std::vector<Value> &value) {

  if (!this->isOpen()) {
    return false;
  }

  if (!this->typeMapping.contains(key)) {
    return false;
  }

  return this->extendingWrite(timestamp, key, value);
}

bool DataManagerHdf::open(std::string name) {
  if (this->isOpen()) {
    return false;
  }

  std::lock_guard<std::mutex> lockGuard(this->dataManagerMutex);

  File *file = new File(name, File::ReadWrite);
  if (!file->isValid()) {
    delete file;
    this->openFlag = false;
    return false;
  }

  this->hdfFile.reset(file);
  this->openFlag = true;

  // Read the structure.
  DataSet dataSetKeys = file->getDataSet("/struct/keys");
  std::vector<std::string> keys;
  dataSetKeys.read(keys);
  DataSet dataSetTypes = file->getDataSet("/struct/types");
  std::vector<int> types;
  dataSetTypes.read(types);

  if (keys.size() != types.size()) {
    return false;
  }

  for (int i = 0; i < keys.size(); i++) {
    this->typeMapping[keys[i]] = static_cast<DataManagerDataType>(types[i]);
  }

  return true;
}

bool DataManagerHdf::open(std::string name, KeyMapping keyMapping, bool force) {
  // Is file already open?
  if (this->isOpen()) {
    // It is. Can not open another file.
    return false;
  }

  std::lock_guard<std::mutex> lockGuard(this->dataManagerMutex);

  // Try to create the file.
  File *file = nullptr;
  bool createdFile = false;
  std::string nameExtension = name + ".hdf";
  if (force) {
    // Just truncate the file.
    file = new File(nameExtension, File::Truncate);
    createdFile = true;
  } else {

    if (std::filesystem::exists(nameExtension)) {
      // The file exists. Try to open the file.
      try {
        file = new File(nameExtension, File::ReadWrite);
        createdFile = false;
      } catch (HighFive::FileException e) {
        // For some reason, the file could not be opened. Rename the existing
        // file and create a new one.
        // TODO: Attempt repair here.
        std::filesystem::rename(nameExtension,
                                std::format("{:%Y%m%d%H%M}", Core::getNow()) +
                                    "_broken_" + nameExtension);
        file = new File(nameExtension, File::Create);
        createdFile = true;
      }
    } else {
      // Create the file.
      file = new File(nameExtension, File::Create);
      createdFile = true;
    }
  }

  // Has the file been created successfully?
  if (file != nullptr && !file->isValid()) {
    // Creation has failed. Return here.
    this->openFlag = false;
    delete file;
    return false;
  }

  // Assign the file pointer.
  this->hdfFile.reset(file);
  this->openFlag = true;

  // If the file has been newely created, the hierarchy has to be created.
  // Otherwise, the key mapping has to be read from the file.
  if (createdFile) {
    // Create the hierarchy. First, create the struture that holds the values
    // ... Use chunking
    DataSetCreateProps props;
    props.add(Chunking(std::vector<hsize_t>{this->defaultChunkingSize, 1}));
    // ... now iterate over the typemapping to create the structures for the
    // data fields and to write into the key/types fields.
    std::vector<std::string> keys;
    std::vector<int> types;
    for (auto keyValuePair : keyMapping) {
      // Create the dataset for the data.
      if (keyValuePair.second == DATAMANAGER_DATA_TYPE_INVALID) {
        continue;
      } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_INT) {
        DataSpace dataspace = DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
        file->createDataSet("/data/" + keyValuePair.first + "/timestamps",
                            dataspace, create_datatype<long long>(), props);
        file->createDataSet("/data/" + keyValuePair.first + "/values",
                            dataspace, create_datatype<int>(), props);
      } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_DOUBLE) {
        DataSpace dataspace = DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
        file->createDataSet("/data/" + keyValuePair.first + "/timestamps",
                            dataspace, create_datatype<long long>(), props);
        file->createDataSet("/data/" + keyValuePair.first + "/values",
                            dataspace, create_datatype<double>(), props);
      } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_COMPLEX) {
        DataSpace dataspaceTimestamps =
            DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
        DataSpace dataspaceValue = DataSpace({0, 2}, {DataSpace::UNLIMITED, 2});
        file->createDataSet("/data/" + keyValuePair.first + "/timestamps",
                            dataspaceTimestamps, create_datatype<long long>(),
                            props);
        file->createDataSet("/data/" + keyValuePair.first + "/values",
                            dataspaceValue, create_datatype<double>(), props);
      } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_STRING) {
        DataSpace dataspace = DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
        file->createDataSet("/data/" + keyValuePair.first + "/timestamps",
                            dataspace, create_datatype<long long>(), props);
        file->createDataSet("/data/" + keyValuePair.first + "/values",
                            dataspace, create_datatype<std::string>(), props);
      } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_SPECTRUM) {
        // Nothing to do. Datasets will be created in setupSpectrumSpecific().
      } else {
        continue;
      }

      keys.push_back(keyValuePair.first);
      types.push_back(static_cast<int>(keyValuePair.second));
    }

    // Write the mapping entry.
    DataSpace dataSpaceKeys =
        DataSpace({keys.size(), 1}, {DataSpace::UNLIMITED, 1});
    DataSet dataSetKeys = file->createDataSet(
        "/struct/keys", dataSpaceKeys, create_datatype<std::string>(), props);
    dataSetKeys.write(keys);

    DataSpace dataSpaceTypes =
        DataSpace({types.size(), 1}, {DataSpace::UNLIMITED, 1});
    DataSet dataSetTypes = file->createDataSet("/struct/types", dataSpaceTypes,
                                               create_datatype<int>(), props);
    dataSetTypes.write(types);
    this->typeMapping = keyMapping;

    this->hdfFile->flush();

  } else {
    // Read the structure.
    DataSet dataSetKeys = file->getDataSet("/struct/keys");
    std::vector<std::string> keys;
    dataSetKeys.read(keys);
    DataSet dataSetTypes = file->getDataSet("/struct/types");
    std::vector<int> types;
    dataSetTypes.read(types);

    if (keys.size() != types.size()) {
      return false;
    }

    for (int i = 0; i < keys.size(); i++) {
      this->typeMapping[keys[i]] = static_cast<DataManagerDataType>(types[i]);

      // If the current key corresponds to a spectrum, read the spectrum mapping
      // aswell.
      if (types[i] == DataManagerDataType::DATAMANAGER_DATA_TYPE_SPECTRUM) {
        DataSet dataSetSpectrumMapping =
            file->getDataSet("/data/" + keys[i] + "/spectrumMapping");
        std::vector<double> spectrum;
        dataSetSpectrumMapping.read(spectrum);
        this->spectrumMapping[keys[i]] = spectrum;
      }
    }
  }

  return true;
}

bool DataManagerHdf::close() {
  if (!this->isOpen()) {
    return false;
  }

  std::lock_guard<std::mutex> lockGuard(this->dataManagerMutex);

  this->hdfFile.reset();
  this->typeMapping.clear();

  return true;
}

DataManagerType DataManagerHdf::getDataManagerType() const {
  return DataManagerType::DATAMANAGER_TYPE_HDF;
}

size_t DataManagerHdf::extendDataSet(const std::string &name, int count) {
  // File has to be open
  if (!this->isOpen()) {
    return -1;
  }

  DataSet dataset = this->hdfFile->getDataSet(name);
  std::vector<size_t> dim = dataset.getDimensions();
  size_t newIdx = dim[0];
  dim[0] = dim[0] + count;
  dataset.resize(dim);

  return newIdx;
}

bool DataManagerHdf::extendingWrite(const std::vector<TimePoint> &timestamp,
                                    const std::string &key,
                                    const std::vector<Value> &value) {

  // Do some checks.
  // File has to be open
  if (!this->isOpen()) {
    return false;
  }
  // Key has to be known.
  if (!this->typeMapping.contains(key)) {
    return false;
  }
  // Timestamp vector and value vector have to be of equal length.
  if (timestamp.size() != value.size()) {
    return false;
  }
  // If the key refers to a spectrum type, the spectrum has to be setup first.
  if (this->typeMapping[key] == DATAMANAGER_DATA_TYPE_SPECTRUM &&
      !this->isSpectrumSetup(key)) {
    return false;
  }

  std::lock_guard<std::mutex> lockGuard(this->dataManagerMutex);

  size_t extendSize = timestamp.size();

  // Extend the dataset, depending on the held data type.
  DataManagerDataType dataType = this->typeMapping[key];
  size_t newIdx;
  this->extendDataSet("/data/" + key + "/values", extendSize);
  newIdx = this->extendDataSet("/data/" + key + +"/timestamps", extendSize);

  if (newIdx == -1) {
    return false;
  }

  // Write the timestamp to the dataset.
  DataSet datasetTimestamps =
      this->hdfFile->getDataSet("/data/" + key + "/timestamps");
  std::vector<long long> timestampVector;
  this->transformTimestampVector(timestamp, timestampVector);
  datasetTimestamps.select({newIdx, 0}, {extendSize, 1}).write(timestampVector);

  // Write the value to the dataset.
  if (DATAMANAGER_DATA_TYPE_INT == dataType) {
    DataSet dataset = this->hdfFile->getDataSet("/data/" + key + "/values");
    std::vector<int> valueVector;
    this->transformValueVector(value, valueVector);
    dataset.select({newIdx, 0}, {extendSize, 1}).write(valueVector);
  }

  else if (DATAMANAGER_DATA_TYPE_DOUBLE == dataType) {
    DataSet dataset = this->hdfFile->getDataSet("/data/" + key + "/values");
    std::vector<double> valueVector;
    this->transformValueVector(value, valueVector);
    dataset.select({newIdx, 0}, {extendSize, 1}).write(valueVector);

  }

  else if (DATAMANAGER_DATA_TYPE_COMPLEX == dataType) {
    DataSet dataset = this->hdfFile->getDataSet("/data/" + key + "/values");
    std::vector<Impedance> valueVector;
    this->transformValueVector(value, valueVector);
    std::vector<std::vector<double>> impedance2dVec;

    Utilities::splitImpedance(valueVector, impedance2dVec);
    dataset.select({newIdx, 0}, {extendSize, 2}).write(impedance2dVec);
  }

  else if (DATAMANAGER_DATA_TYPE_STRING == dataType) {
    DataSet dataset = this->hdfFile->getDataSet("/data/" + key + "/values");
    std::vector<std::string> valueVector;
    this->transformValueVector(value, valueVector);
    dataset.select({newIdx, 0}, {extendSize, 1}).write(valueVector);
  }

  else if (DATAMANAGER_DATA_TYPE_SPECTRUM == dataType) {
    DataSet dataset = this->hdfFile->getDataSet("/data/" + key + "/values");
    std::vector<ImpedanceSpectrum> valueVector;
    this->transformValueVector(value, valueVector);
    std::vector<std::vector<std::vector<double>>> valuesArray;
    Utilities::splitImpedanceSpectrum(valueVector, valuesArray);
    size_t frequencyCount = this->spectrumMapping[key].size();
    dataset.select({newIdx, 0, 0}, {extendSize, frequencyCount, 2})
        .write(valuesArray);
  }

  else {
    return false;
  }

  this->hdfFile->flush();
  return true;
}

bool DataManagerHdf::createKey(std::string key, DataManagerDataType dataType) {
  if (this->typeMapping.contains(key)) {
    return false;
  }
  if (!this->isOpen()) {
    return false;
  }

  this->typeMapping[key] = dataType;

  std::lock_guard<std::mutex> lockGuard(this->dataManagerMutex);

  // Create the hierarchy. First, create the structure that holds the values ...
  // Use chunking
  DataSetCreateProps props;
  props.add(Chunking(std::vector<hsize_t>{this->defaultChunkingSize, 1}));
  // ... now iterate over the typemapping to create the structures for the data
  // fields and to write into the key/types fields.
  // Create the dataset for the data.
  if (dataType == DATAMANAGER_DATA_TYPE_INT) {
    DataSpace dataspace = DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
    this->hdfFile->createDataSet("/data/" + key + "/timestamps", dataspace,
                                 create_datatype<long long>(), props);
    this->hdfFile->createDataSet("/data/" + key + "/values", dataspace,
                                 create_datatype<int>(), props);
  } else if (dataType == DATAMANAGER_DATA_TYPE_DOUBLE) {
    DataSpace dataspace = DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
    this->hdfFile->createDataSet("/data/" + key + "/timestamps", dataspace,
                                 create_datatype<long long>(), props);
    this->hdfFile->createDataSet("/data/" + key + "/values", dataspace,
                                 create_datatype<double>(), props);
  } else if (dataType == DATAMANAGER_DATA_TYPE_COMPLEX) {
    DataSpace dataspaceTimestamps =
        DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
    DataSpace dataspaceValue = DataSpace({0, 2}, {DataSpace::UNLIMITED, 2});
    this->hdfFile->createDataSet("/data/" + key + "/timestamps",
                                 dataspaceTimestamps,
                                 create_datatype<long long>(), props);
    this->hdfFile->createDataSet("/data/" + key + "/values", dataspaceValue,
                                 create_datatype<double>(), props);
  } else if (dataType == DATAMANAGER_DATA_TYPE_STRING) {
    DataSpace dataspace = DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
    this->hdfFile->createDataSet("/data/" + key + "/timestamps", dataspace,
                                 create_datatype<long long>(), props);
    this->hdfFile->createDataSet("/data/" + key + "/values", dataspace,
                                 create_datatype<std::string>(), props);
  } else if (dataType == DATAMANAGER_DATA_TYPE_SPECTRUM) {
    // Dataset will be created in setupSpectrumSpecific(). For now, create the
    // node.
    this->hdfFile->createGroup("/data/" + key);
  } else {
    return false;
  }

  // Read back the key and types field and append to it.
  DataSet keyDataset = this->hdfFile->getDataSet("/struct/keys");
  std::vector<std::string> keys;
  keyDataset.read(keys);
  keys.push_back(key);
  this->extendDataSet("/struct/keys", 1);
  keyDataset.write(keys);

  DataSet typeDataset = this->hdfFile->getDataSet("/struct/types");
  std::vector<int> types;
  typeDataset.read(types);
  types.push_back(static_cast<int>(dataType));
  this->extendDataSet("/struct/types", 1);
  typeDataset.write(types);

  this->hdfFile->flush();

  return true;
}

template <class T>
bool DataManagerHdf::transformValueVector(const std::vector<Value> &origVec,
                                          std::vector<T> &transformedVec) {
  transformedVec.reserve(origVec.size());
  for (auto &origVecValue : origVec) {
    transformedVec.emplace_back(std::get<T>(origVecValue));
  }
  return true;
}

bool DataManagerHdf::transformTimestampVector(
    const std::vector<TimePoint> &origVec,
    std::vector<long long> &transformedVec) {

  transformedVec.reserve(origVec.size());
  for (auto &origVecValue : origVec) {
    transformedVec.emplace_back(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            origVecValue.time_since_epoch())
            .count());
  }
  return true;
}

bool DataManagerHdf::setupSpectrumSpecific(std::string key,
                                           std::vector<double> frequencies) {
  if (!this->isOpen()) {
    return false;
  }

  std::lock_guard<std::mutex> lockGuard(this->dataManagerMutex);

  // Create the dataset for the spectrum.
  size_t frequencyCount = frequencies.size();
  DataSetCreateProps propsValues;
  propsValues.add(Chunking(std::vector<hsize_t>{1, frequencyCount, 2}));
  DataSpace dataspaceSpectrum = DataSpace(
      {0, frequencyCount, 2}, {DataSpace::UNLIMITED, frequencyCount, 2});
  this->hdfFile->createDataSet("/data/" + key + "/values", dataspaceSpectrum,
                               create_datatype<double>(), propsValues);
  // Create the dataset for the timestamps.
  DataSetCreateProps propsTimestamps;
  propsTimestamps.add(
      Chunking(std::vector<hsize_t>{this->defaultChunkingSize, 1}));
  DataSpace dataspaceTimestamps = DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
  this->hdfFile->createDataSet("/data/" + key + "/timestamps",
                               dataspaceTimestamps,
                               create_datatype<long long>(), propsTimestamps);
  // Create the dataset for the spectrum mapping.
  DataSet datasetSpectrumMapping = this->hdfFile->createDataSet(
      "/data/" + key + "/spectrumMapping", DataSpace::From(frequencies),
      create_datatype<double>());

  datasetSpectrumMapping.write(frequencies);
  this->hdfFile->flush();

  return true;
}

TimerangeMapping DataManagerHdf::getTimerangeMapping() const {
  // Get the lock.
  std::lock_guard<std::mutex> lockGuard(this->dataManagerMutex);

  // Iterate over the known keys.
  TimerangeMapping retVal;
  for (auto &keyValuePair : this->typeMapping) {
    // Read timestamps for the key.
    DataSet datasetTimestamps = this->hdfFile->getDataSet(
        "/data/" + keyValuePair.first + "/timestamps");
    hsize_t datasetSize = datasetTimestamps.getDimensions().front();
    if (datasetSize <= 0) {
      // This dataset seems to be empty. indicate that, by writing zeroes to the
      // time range mapping.
      retVal[keyValuePair.first] =
          std::make_pair(TimePoint(std::chrono::milliseconds(0)),
                         TimePoint(std::chrono::milliseconds(0)));

      continue;
    }
    long long timestampBegin;
    datasetTimestamps.select({0, 0}, {1, 1}).read(timestampBegin);
    long long timestampEnd;
    datasetTimestamps.select({datasetSize - 1, 0}, {1, 1}).read(timestampEnd);

    retVal[keyValuePair.first] =
        std::make_pair(TimePoint(std::chrono::milliseconds(timestampBegin)),
                       TimePoint(std::chrono::milliseconds(timestampEnd)));
  }

  return retVal;
}

void DataManagerHdf::traverseNodes(Group &node,
                                   std::vector<std::string> &nodeNames) {

  // Get the names of the children.
  auto childrenNames = node.listObjectNames();

  const std::string nodePath = node.getPath();
  for (auto &childrenName : childrenNames) {
    // Prepend the path of the parent to the child.
    childrenName = nodePath + "/" + childrenName;

    // Is this child a group?
    if (ObjectType::Group != this->hdfFile->getObjectType(childrenName)) {
      continue;
    }

    nodeNames.push_back(childrenName);
    Group subNode = this->hdfFile->getGroup(childrenName);

    traverseNodes(subNode, nodeNames);
  }
}

std::map<std::string, Devices::DeviceType>
DataManagerHdf::filterMeasurements(std::vector<std::string> &groupNames) {
  std::map<std::string, Devices::DeviceType> retVal;

  for (auto &groupName : groupNames) {
    Group group = this->hdfFile->getGroup(groupName);
    if (group.hasAttribute(DataManager::DATA_MANAGER_DEVICETYPE_ATTR_NAME)) {
      retVal[groupName] = static_cast<Devices::DeviceType>(
          group.getAttribute(DataManager::DATA_MANAGER_DEVICETYPE_ATTR_NAME)
              .read<int>());
    }
  }

  return retVal;
}

void DataManagerHdf::printImpdedanceSpectrum(std::stringstream &ss,
                                             HighFive::DataSet &spectrumMapping,
                                             HighFive::DataSet &timestamp,
                                             HighFive::DataSet &spectra,
                                             const std::string &impdenaceFormat,
                                             char separator) {

  std::vector<double> spectrumMappingVec;
  spectrumMapping.read(spectrumMappingVec);

  // Print the header.

  ss << "timestamps" << separator;
  for (auto spectrumPoint : spectrumMappingVec) {
    if (impdenaceFormat == "cartesian") {
      ss << spectrumPoint << "_real" << separator << spectrumPoint << "_imag"
         << separator;
    } else {
      ss << spectrumPoint << "_value" << separator << spectrumPoint << "_phase"
         << separator;
    }
  }
  ss << std::endl;

  auto dims = timestamp.getDimensions();
  for (size_t i = 0; i < dims[0]; i++) {

    auto timestampInt = timestamp.select({i, 0}, {1, 1}).read<long long>();
    auto spectrumArray =
        spectra.select({i, 0, 0}, {1, spectrumMappingVec.size(), 2})
            .read<std::vector<std::vector<std::vector<double>>>>();

    ss << timestampInt << separator;

    if (impdenaceFormat == "cartesian") {
      for (auto frequencyTuple : spectrumArray[0]) {
        ss << frequencyTuple[0] << separator << frequencyTuple[1] << separator;
      }
    } else {
      for (auto frequencyTuple : spectrumArray[0]) {
        double value = std::sqrt(frequencyTuple[0] * frequencyTuple[0] +
                                 frequencyTuple[1] * frequencyTuple[1]);
        double phase = std::atan(frequencyTuple[1] / frequencyTuple[0]);

        ss << value << separator << phase << separator;
      }
    }

    ss << std::endl;
  }
}

void DataManagerHdf::printPumpData(std::stringstream &ss,
                                   HighFive::DataSet &currPressureValues,
                                   HighFive::DataSet &currPressureTimestamps,
                                   HighFive::DataSet &setPressureValue,
                                   HighFive::DataSet &setPressureTimestamps,
                                   char separator) {

  // Print the header.
  ss << "timestamps" << separator << "current_pressure" << separator
     << "set_pressure" << separator << std::endl;

  const auto currDims = currPressureTimestamps.getDimensions();
  const auto setDims = setPressureTimestamps.getDimensions();
  double actualSetPressure = 0.0;
  double actualCurrentPressure = 0.0;
  size_t actualSetPressureIdx = 0;
  size_t actualCurrentPressureIdx = 0;
  auto actualSetPressureTimestamp =
      setPressureTimestamps.select({actualSetPressureIdx, 0}, {1, 1})
          .read<long long>();
  bool noMoreSetTimestamps = false;

  // Run as long as there are current pressure values to print.
  while (actualCurrentPressureIdx < currDims[0]) {
    // Get the current pressure timestamp.
    const auto currPressureTimestamp =
        currPressureTimestamps.select({actualCurrentPressureIdx, 0}, {1, 1})
            .read<long long>();

    // Is the current pressure timestamp older than the actual set pressure
    // timestamp?
    if (currPressureTimestamp <= actualSetPressureTimestamp ||
        noMoreSetTimestamps) {
      // It is. Print the actual current pressure value and print set old set
      // pressure value.
      actualCurrentPressure =
          currPressureValues.select({actualCurrentPressureIdx, 0}, {1, 1})
              .read<double>();
      ss << currPressureTimestamp << separator << actualCurrentPressure
         << separator << actualSetPressure << separator << std::endl;

      actualCurrentPressureIdx++;
    } else {
      // It is not. Fetch a new set pressure and print it. Use the old current
      // pressure value.
      actualSetPressure =
          setPressureValue.select({actualSetPressureIdx, 0}, {1, 1})
              .read<double>();
      ss << actualSetPressureTimestamp << separator << actualCurrentPressure
         << separator << actualSetPressure << separator << std::endl;

      if (actualSetPressureIdx < setDims[0] - 1) {
        actualSetPressureIdx++;
        actualSetPressureTimestamp =
            setPressureTimestamps.select({actualSetPressureIdx, 0}, {1, 1})
                .read<long long>();
      } else {
        noMoreSetTimestamps = true;
      }
    }
  }
}

bool DataManagerHdf::writeToCsv(std::map<std::string, std::stringstream *> &ss,
                                char separator,
                                const std::string &impedanceFormat) {
  if (!this->isOpen()) {
    return false;
  }

  Group rootNode = this->hdfFile->getGroup("/data");
  std::vector<std::string> groupNames;
  traverseNodes(rootNode, groupNames);

  std::map<std::string, Devices::DeviceType> measurements =
      filterMeasurements(groupNames);

  for (auto &measurement : measurements) {

    if (measurement.second == Devices::DeviceType::IMPEDANCE_SPECTROMETER) {
      std::stringstream *currentStream = new std::stringstream();

      DataSet spectrumMapping =
          this->hdfFile->getDataSet(measurement.first + "/spectrumMapping");
      DataSet timestamps =
          this->hdfFile->getDataSet(measurement.first + "/timestamps");
      DataSet spectra =
          this->hdfFile->getDataSet(measurement.first + "/values");
      this->printImpdedanceSpectrum(*currentStream, spectrumMapping, timestamps,
                                    spectra, impedanceFormat, separator);

      ss[measurement.first] = currentStream;
    } else if (measurement.second == Devices::DeviceType::PUMP_CONTROLLER) {

      constexpr size_t COUNT_CHANNELS = 4;
      for (size_t i = 1; i <= COUNT_CHANNELS; i++) {
        std::stringstream *currentStream = new std::stringstream();

        DataSet currPressTimestamps = this->hdfFile->getDataSet(
            measurement.first + "/channel" + std::to_string(i) +
            "/currPressure/timestamps");
        DataSet currPressValues = this->hdfFile->getDataSet(
            measurement.first + "/channel" + std::to_string(i) +
            "/currPressure/values");
        DataSet setPressTimestamps = this->hdfFile->getDataSet(
            measurement.first + "/channel" + std::to_string(i) +
            "/setpoint/timestamps");
        DataSet setPressValues =
            this->hdfFile->getDataSet(measurement.first + "/channel" +
                                      std::to_string(i) + "/setpoint/values");

        this->printPumpData(*currentStream, currPressValues,
                            currPressTimestamps, setPressValues,
                            setPressTimestamps, separator);

        ss[measurement.first + "_ch" + std::to_string(i)] = currentStream;
      }
    }
  }

  return true;
}

bool DataManagerHdf::createGroup(
    const std::string &groupName, const std::map<std::string, int> &intProps,
    const std::map<std::string, double> &doubleProps,
    const std::map<std::string, std::string> &strProps) {

  // Try to get the group

  // Set the device type as attribute.
  Group group;
  try {
    group = this->hdfFile->getGroup("/data/" + groupName);
  } catch (GroupException &e) {
    // Group seems to
    group = this->hdfFile->createGroup("/data/" + groupName);
  } catch (...) {
    return false;
  }

  for (auto &intProp : intProps) {
    if (!group.hasAttribute(intProp.first)) {
      group.createAttribute<int>(intProp.first, intProp.second);
    }
  }

  for (auto &doubleProp : doubleProps) {
    if (!group.hasAttribute(doubleProp.first)) {
      group.createAttribute<double>(doubleProp.first, doubleProp.second);
    }
  }

  for (auto &strProp : strProps) {
    if (!group.hasAttribute(strProp.first)) {
      group.createAttribute<std::string>(strProp.first, strProp.second);
    }
  }

  return true;
}
