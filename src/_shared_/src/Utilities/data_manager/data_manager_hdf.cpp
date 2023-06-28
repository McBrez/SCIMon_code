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

  DataSet datasetTimestamps =
      this->hdfFile->getDataSet("data/timestamps/" + key);

  for (size_t i = 0; i < datasetTimestamps.getElementCount(); i++) {
    long long timestampLongRead;
    datasetTimestamps.select({i}, {1}).read(timestampLongRead);
    TimePoint timestampRead =
        TimePoint(std::chrono::milliseconds(timestampLongRead));
    if (timestampRead == timestamp) {
      DataManagerDataType dataType = this->typeMapping[key];
      if (DATAMANAGER_DATA_TYPE_INT == dataType) {
        DataSet datasetValues = this->hdfFile->getDataSet("data/values/" + key);
        value = datasetValues.select({i}, {1}).read<int>();

        return true;
      } else if (DATAMANAGER_DATA_TYPE_DOUBLE == dataType) {
        DataSet datasetValues = this->hdfFile->getDataSet("data/values/" + key);
        value = datasetValues.select({i}, {1}).read<double>();

        return true;
      } else if (DATAMANAGER_DATA_TYPE_COMPLEX == dataType) {
        DataSet datasetValuesImag =
            this->hdfFile->getDataSet("data/values/" + key + "_imag");
        DataSet datasetValuesReal =
            this->hdfFile->getDataSet("data/values/" + key + "_real");
        Impedance impedance(datasetValuesReal.select({i}, {1}).read<double>(),
                            datasetValuesImag.select({i}, {1}).read<double>());
        value = impedance;

        return true;

      } else if (DATAMANAGER_DATA_TYPE_STRING == dataType) {
        DataSet datasetValues = this->hdfFile->getDataSet("data/values/" + key);
        value = datasetValues.select({i}, {1}).read<std::string>();

        return true;

      } else {
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

  DataSet datasetTimestamps =
      this->hdfFile->getDataSet("data/timestamps/" + key);

  // Iterate over the whole dataset and find the index margins of the requested
  // time frame.
  size_t idxFrom = -1;
  size_t idxTo = -1;
  for (size_t i = 0; i < datasetTimestamps.getElementCount(); i++) {
    long long timestampLongRead;
    datasetTimestamps.select({i}, {1}).read(timestampLongRead);
    TimePoint timestampCurrent((std::chrono::milliseconds(timestampLongRead)));
    if (timestampCurrent >= from && idxFrom == -1) {
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
  datasetTimestamps.select({idxFrom}, {idxTo - idxFrom}).read(timestampsRaw);
  timestamps.reserve(timestampsRaw.size());
  for (auto timestampsRawValue : timestampsRaw) {
    timestamps.emplace_back(
        TimePoint(std::chrono::milliseconds(timestampsRawValue)));
  }

  // Read from the data set and construct a std::vector<Value>.
  DataManagerDataType dataType = this->typeMapping[key];
  if (DATAMANAGER_DATA_TYPE_INT == dataType) {
    DataSet datasetValues = this->hdfFile->getDataSet("data/values/" + key);
    std::vector<int> rawVector =
        datasetValues.select({idxFrom}, {idxTo - idxFrom})
            .read<std::vector<int>>();
    value.reserve(rawVector.size());
    for (auto rawVectorValue : rawVector) {
      value.emplace_back(Value(rawVectorValue));
    }

    return true;
  }

  else if (DATAMANAGER_DATA_TYPE_DOUBLE == dataType) {
    DataSet datasetValues = this->hdfFile->getDataSet("data/values/" + key);
    std::vector<double> rawVector =
        datasetValues.select({idxFrom}, {idxTo - idxFrom})
            .read<std::vector<double>>();
    value.reserve(rawVector.size());
    for (auto rawVectorValue : rawVector) {
      value.emplace_back(Value(rawVectorValue));
    }

    return true;
  }

  else if (DATAMANAGER_DATA_TYPE_COMPLEX == dataType) {
    DataSet datasetValuesImag =
        this->hdfFile->getDataSet("data/values/" + key + "_imag");
    DataSet datasetValuesReal =
        this->hdfFile->getDataSet("data/values/" + key + "_real");
    std::vector<double> rawRealValues =
        datasetValuesReal.select({idxFrom}, {idxTo - idxFrom})
            .read<std::vector<double>>();
    std::vector<double> rawImagValues =
        datasetValuesImag.select({idxFrom}, {idxTo - idxFrom})
            .read<std::vector<double>>();

    value.reserve(rawRealValues.size());
    for (int i = 0; i < rawRealValues.size(); i++) {
      value.emplace_back(Impedance(rawRealValues[i], rawImagValues[i]));
    }

    return true;
  }

  else if (DATAMANAGER_DATA_TYPE_STRING == dataType) {
    DataSet datasetValues = this->hdfFile->getDataSet("data/values/" + key);
    std::vector<std::string> rawVector =
        datasetValues.select({idxFrom}, {idxTo - idxFrom})
            .read<std::vector<std::string>>();
    value.reserve(rawVector.size());
    for (auto rawVectorValue : rawVector) {
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

  return this->extendingWrite(timestamp, key, value);
}

bool DataManagerHdf::open(std::string name) {
  if (this->isOpen()) {
    return false;
  }

  File *file = new File(name, File::ReadWrite);
  if (!file->isValid()) {
    this->openFlag = false;
    return false;
  }

  this->hdfFile.reset(file);
  this->openFlag = true;

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

  // Create the hierarchy. First, create the struture that holds the data type
  // mapping ...
  DataSpace dataspaceStruct = DataSpace({this->typeMapping.size()});
  file->createDataSet("struct/keys", dataspaceStruct,
                      create_datatype<std::string>());
  file->createDataSet("struct/types", dataspaceStruct, create_datatype<int>());
  // Use chunking
  DataSetCreateProps props;
  props.add(Chunking(std::vector<hsize_t>{1, 1024}));
  // ... now iterate over the typemapping to create the structures for the data
  // fields and to write into the key/types fields.
  DataSpace dataspace = DataSpace({1, 1}, {1, DataSpace::UNLIMITED});
  for (auto keyValuePair : keyMapping) {
    // Create the dataset for the data.
    if (keyValuePair.second == DATAMANAGER_DATA_TYPE_INVALID) {
      continue;
    } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_INT) {
      file->createDataSet("data/timestamps/" + keyValuePair.first, dataspace,
                          create_datatype<long long>(), props);
      file->createDataSet("data/values/" + keyValuePair.first, dataspace,
                          create_datatype<int>(), props);
    } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_DOUBLE) {
      file->createDataSet("data/timestamps/" + keyValuePair.first, dataspace,
                          create_datatype<long long>(), props);
      file->createDataSet("data/values/" + keyValuePair.first, dataspace,
                          create_datatype<double>(), props);
    } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_COMPLEX) {
      file->createDataSet("data/timestamps/" + keyValuePair.first, dataspace,
                          create_datatype<long long>(), props);
      file->createDataSet("data/values/" + keyValuePair.first + "_real",
                          dataspace, create_datatype<double>(), props);
      file->createDataSet("data/values/" + keyValuePair.first + "_imag",
                          dataspace, create_datatype<double>(), props);
    } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_STRING) {
      file->createDataSet("data/timestamps/" + keyValuePair.first, dataspace,
                          create_datatype<long long>(), props);
      file->createDataSet("data/values/" + keyValuePair.first, dataspace,
                          create_datatype<std::string>(), props);
    } else {
      continue;
    }

    // Write the mapping entry.
    size_t newIdx = this->extendDataSet("struct/keys", 1);
    file->getDataSet("struct/keys")
        .select({newIdx}, {1})
        .write(keyValuePair.first);
    newIdx = this->extendDataSet("struct/types", 1);
    file->getDataSet("struct/types")
        .select({newIdx}, {1})
        .write(static_cast<int>(keyValuePair.second));
  }

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
  size_t newIdx = dim[0] + count;
  dataset.resize({newIdx});

  return newIdx;
}

bool DataManagerHdf::extendingWrite(TimePoint timestamp, const std::string &key,
                                    const Value &value) {

  // Do some checks.
  // File has to be open
  if (!this->isOpen()) {
    return false;
  }
  // Key has to be known.
  if (this->typeMapping.contains(key)) {
    return false;
  }

  // Extend the dataset, depending on the held data type.
  DataManagerDataType dataType = this->typeMapping[key];
  size_t newIdx;
  if (DATAMANAGER_DATA_TYPE_COMPLEX == dataType) {
    this->extendDataSet("data/values/" + key + "_real", 1);
    this->extendDataSet("data/values/" + key + "_imag", 1);
    newIdx = this->extendDataSet("data/timestamps/" + key, 1);
  } else {
    this->extendDataSet("data/values/" + key, 1);
    newIdx = this->extendDataSet("data/timestamps/" + key, 1);
  }

  // Write to the dataset
  if (DATAMANAGER_DATA_TYPE_INT == dataType) {
    DataSet dataset = this->hdfFile->getDataSet("data/values/" + key);
    dataset.select({newIdx}, {1}).write(std::get<int>(value));
    DataSet datasetTimestamps =
        this->hdfFile->getDataSet("data/timestamps/" + key);
    datasetTimestamps.select({newIdx}, {1})
        .write(timestamp.time_since_epoch().count());
  }

  else if (DATAMANAGER_DATA_TYPE_DOUBLE == dataType) {
    DataSet dataset = this->hdfFile->getDataSet("data/values/" + key);
    dataset.select({newIdx}, {1}).write(std::get<double>(value));
    DataSet datasetTimestamps =
        this->hdfFile->getDataSet("data/timestamps/" + key);
    datasetTimestamps.select({newIdx}, {1})
        .write(timestamp.time_since_epoch().count());
  }

  else if (DATAMANAGER_DATA_TYPE_COMPLEX == dataType) {
    DataSet datasetReal =
        this->hdfFile->getDataSet("data/values/" + key + "_real");
    DataSet datasetImag =
        this->hdfFile->getDataSet("data/values/" + key + "_imag");
    datasetReal.select({newIdx}, {1}).write(std::get<Impedance>(value).real());
    datasetImag.select({newIdx}, {1}).write(std::get<Impedance>(value).imag());
    DataSet datasetTimestamps =
        this->hdfFile->getDataSet("data/timestamps/" + key);
    datasetTimestamps.select({newIdx}, {1})
        .write(timestamp.time_since_epoch().count());
  }

  else if (DATAMANAGER_DATA_TYPE_STRING == dataType) {
    DataSet dataset = this->hdfFile->getDataSet("data/values/" + key);
    dataset.select({newIdx}, {1}).write(std::get<std::string>(value));
    DataSet datasetTimestamps =
        this->hdfFile->getDataSet("data/timestamps/" + key);
    datasetTimestamps.select({newIdx}, {1})
        .write(timestamp.time_since_epoch().count());
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

  return true;
}
