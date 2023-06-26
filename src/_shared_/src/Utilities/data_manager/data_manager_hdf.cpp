// Project includes
#include <data_manager_hdf.hpp>

using namespace Utilities;
using namespace HighFive;

DataManagerHdf::~DataManagerHdf() { this->close(); }

bool DataManagerHdf::read(TimePoint timestamp, std::string key, Value &value) {
  if (!this->isOpen()) {
    return false;
  }
}

bool DataManagerHdf::read(TimePoint from, TimePoint to, std::string key,
                          Value &value) {

  if (!this->isOpen()) {
    return false;
  }
}

bool DataManagerHdf::write(TimePoint timestamp, std::string key,
                           const Value &value) {

  if (!this->isOpen()) {
    return false;
  }

  if (!this->typeMapping.contains(key)) {
    return false;
  }

  DataManagerDataType dataType = this->typeMapping[key];
  if (DATAMANAGER_DATA_TYPE_INT == dataType) {

  } else if (DATAMANAGER_DATA_TYPE_DOUBLE == dataType) {

  } else if (DATAMANAGER_DATA_TYPE_COMPLEX == dataType) {

  } else if (DATAMANAGER_DATA_TYPE_STRING == dataType) {

  } else {
    return false;
  }
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

  // Create the hierarchy.
  DataSpace dataspaceStruct = DataSpace({1, this->typeMapping.size()});
  file->createDataSet("struct/keys", dataspaceStruct,
                      create_datatype<std::string>());
  file->createDataSet("struct/types", dataspaceStruct, create_datatype<int>());
  DataSpace dataspace = DataSpace({1, 1}, {1, DataSpace::UNLIMITED});
  for (auto keyValuePair : keyMapping) {
    if (keyValuePair.second == DATAMANAGER_DATA_TYPE_INVALID) {
      continue;
    } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_INT) {
      file->createDataSet("timestamps/" + keyValuePair.first, dataspace,
                          create_datatype<double>());
      file->createDataSet("data/" + keyValuePair.first, dataspace,
                          create_datatype<int>());
    } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_DOUBLE) {
      file->createDataSet("timestamps/" + keyValuePair.first, dataspace,
                          create_datatype<double>());
      file->createDataSet("data/" + keyValuePair.first, dataspace,
                          create_datatype<double>());
    } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_COMPLEX) {
      file->createDataSet("timestamps/" + keyValuePair.first, dataspace,
                          create_datatype<int>());
      file->createDataSet("data/" + keyValuePair.first + "_real", dataspace,
                          create_datatype<double>());
      file->createDataSet("data/" + keyValuePair.first + "_imag", dataspace,
                          create_datatype<double>());
    } else if (keyValuePair.second == DATAMANAGER_DATA_TYPE_STRING) {
      file->createDataSet("timestamps/" + keyValuePair.first, dataspace,
                          create_datatype<int>());
      file->createDataSet("data/" + keyValuePair.first, dataspace,
                          create_datatype<std::string>());
    } else {
      continue;
    }
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
