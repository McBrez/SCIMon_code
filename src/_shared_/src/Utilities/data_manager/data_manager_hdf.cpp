#include <easylogging++.h>>

// Project includes
#include <data_manager_hdf.hpp>

using namespace Utilities;
using namespace HighFive;

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

  DataSet datasetTimestamps =
      this->hdfFile->getDataSet("/data/" + key + "/timestamps");

  for (size_t i = 0; i < datasetTimestamps.getElementCount(); i++) {
    long long timestampLongRead;
    datasetTimestamps.select({i, 0}, {1, 1}).read(timestampLongRead);
    TimePoint timestampRead =
        TimePoint(std::chrono::milliseconds(timestampLongRead));
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

  DataSet datasetTimestamps =
      this->hdfFile->getDataSet("/data/" + key + "/timestamps");

  // Iterate over the whole dataset and find the index margins of the requested
  // time frame.
  size_t idxFrom = -1;
  size_t idxTo = -1;
  for (size_t i = 0; i < datasetTimestamps.getElementCount(); i++) {
    long long timestampLongRead;
    datasetTimestamps.select({i, 0}, {1, 1}).read(timestampLongRead);
    TimePoint timestampCurrent((std::chrono::milliseconds(timestampLongRead)));
    if (timestampCurrent >= from && timestampCurrent <= to && idxFrom == -1) {
      idxFrom = i;
      continue;
    }
    if (timestampCurrent >= to && idxTo == -1) {
      idxTo = i;
      break;
    }
  }

  // Check if the indices have been found. idxFrom has to greater or equal 0.
  // idxTo optionally needs to be greater or equal 0. An idxTo of -1 means, that
  // the idx would lie beyond the range of the dataset. In that case, just take
  // the last element as idxTo.
  if (idxFrom == -1) {
    // Return empty vector.
    value = std::vector<Value>();

    return true;
  }
  if (idxTo == -1) {
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

  // Try to create the file.
  File *file;
  if (force) {
    file = new File(name, File::Truncate);
  } else {
    file = new File(name, File::Create);
  }

  // Has the file been created?
  if (!file->isValid()) {
    // Creation has failed. Return here.
    this->openFlag = false;
    delete file;
    return false;
  }

  // Assign the file pointer.
  this->hdfFile.reset(file);
  this->openFlag = true;

  // Create the hierarchy. First, create the struture that holds the values ...
  // Use chunking
  DataSetCreateProps props;
  props.add(Chunking(std::vector<hsize_t>{1024, 1}));
  // ... now iterate over the typemapping to create the structures for the data
  // fields and to write into the key/types fields.
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
      file->createDataSet("/data/" + keyValuePair.first + "/values", dataspace,
                          create_datatype<int>(), props);
    } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_DOUBLE) {
      DataSpace dataspace = DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
      file->createDataSet("/data/" + keyValuePair.first + "/timestamps",
                          dataspace, create_datatype<long long>(), props);
      file->createDataSet("/data/" + keyValuePair.first + "/values", dataspace,
                          create_datatype<double>(), props);
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
      file->createDataSet("/data/" + keyValuePair.first + "/values", dataspace,
                          create_datatype<std::string>(), props);
    } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_SPECTRUM) {
      // Nothing to do. Datasets will be created in setupSpectrumSpecific().
    }

    else {
      continue;
    }

    keys.push_back(keyValuePair.first);
    types.push_back(static_cast<int>(keyValuePair.second));
  }

  // Write the mapping entry.
  DataSet dataSetKeys = file->createDataSet(
      "/struct/keys", DataSpace::From(keys), create_datatype<std::string>());
  dataSetKeys.write(keys);
  DataSet dataSetTypes = file->createDataSet(
      "/struct/types", DataSpace::From(types), create_datatype<int>());
  dataSetTypes.write(types);
  this->typeMapping = keyMapping;

  return true;
}

bool DataManagerHdf::close() {
  if (!this->isOpen()) {
    return false;
  }

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

  size_t extendSize = timestamp.size();

  // Extend the dataset, depending on the held data type.
  DataManagerDataType dataType = this->typeMapping[key];
  size_t newIdx;
  this->extendDataSet("/data/" + key + "/values", extendSize);
  newIdx = this->extendDataSet("/data/" + key + +"/timestamps", extendSize);

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
    std::vector<double> realVec;
    std::vector<double> imagVec;
    Utilities::splitImpedance(valueVector, realVec, imagVec);
    dataset.select({newIdx, 0}, {extendSize, 2})
        .write(std::vector<std::vector<double>>{{realVec}, {imagVec}});
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

  // Create the hierarchy. First, create the struture that holds the values ...
  // Use chunking
  DataSetCreateProps props;
  props.add(Chunking(std::vector<hsize_t>{1024, 1}));
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
    // Nothing to do. Dataset will be created in setupSpectrumSpecific().
  }

  else {
    return false;
  }

  // Read back the key and types field and append to it.
  DataSet keyDataset = this->hdfFile->getDataSet("/struct/keys");
  std::vector<std::string> keys;
  keyDataset.read(keys);
  keys.push_back(key);
  keyDataset.write(keys);

  DataSet typeDataset = this->hdfFile->getDataSet("/struct/types");
  std::vector<int> types;
  typeDataset.read(types);
  types.push_back(static_cast<int>(dataType));
  typeDataset.write(types);

  return true;
}

template <class T>
bool DataManagerHdf::transformValueVector(const std::vector<Value> &origVec,
                                          std::vector<T> &transformedVec) {
  transformedVec.reserve(origVec.size());
  for (auto origVecValue : origVec) {
    transformedVec.emplace_back(std::get<T>(origVecValue));
  }
  return true;
}

bool DataManagerHdf::transformTimestampVector(
    const std::vector<TimePoint> &origVec,
    std::vector<long long> &transformedVec) {

  transformedVec.reserve(origVec.size());
  for (auto origVecValue : origVec) {
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
  propsTimestamps.add(Chunking(std::vector<hsize_t>{1024, 1}));
  DataSpace dataspaceTimestamps = DataSpace({0, 1}, {DataSpace::UNLIMITED, 1});
  this->hdfFile->createDataSet("/data/" + key + "/timestamps",
                               dataspaceTimestamps,
                               create_datatype<long long>(), propsTimestamps);
  // Create the dataset for the spectrum mapping.
  DataSet datasetSpectrumMapping = this->hdfFile->createDataSet(
      "/data/" + key + "/spectrumMapping", DataSpace::From(frequencies),
      create_datatype<double>());

  datasetSpectrumMapping.write(frequencies);

  return true;
}
