// Project includes
#include <data_manager.hpp>
#include <data_manager_hdf.hpp>

using namespace Utilities;

const std::string DataManager::DATA_MANAGER_DEVICETYPE_ATTR_NAME =
    "device_type";

DataManager::~DataManager() {}

bool DataManager::isOpen() const { return this->openFlag; }

KeyMapping DataManager::getKeyMapping() const { return this->typeMapping; }

DataManager *DataManager::getDataManager(DataManagerType dataManagerType) {
  if (DataManagerType::DATAMANAGER_TYPE_HDF == dataManagerType) {
    return new DataManagerHdf();
  } else {
    return nullptr;
  }
}

Duration DataManager::timePointDiff(TimePoint a, TimePoint b) {

  Duration diff = std::chrono::duration_cast<Duration>(a - b);
  if (diff.count() < 0) {
    return diff * -1;
  } else {
    return diff;
  }
}

bool DataManager::setupSpectrum(std::string key,
                                std::vector<double> frequencies) {
  if (!this->getKeyMapping().contains(key)) {
    return false;
  }
  if (this->getKeyMapping()[key] !=
      DataManagerDataType::DATAMANAGER_DATA_TYPE_SPECTRUM) {
    return false;
  }
  if (this->spectrumMapping.contains(key)) {
    return false;
  }
  if (this->isSpectrumSetup(key)) {
    return false;
  }
  this->spectrumMapping[key] = frequencies;

  return this->setupSpectrumSpecific(key, frequencies);
}

SpectrumMapping DataManager::getSpectrumMapping() const {
  return this->spectrumMapping;
}

bool DataManager::isSpectrumSetup(std::string key) {
  if (!this->spectrumMapping.contains(key)) {
    return false;
  }

  return this->spectrumMapping[key].size() > 0;
}
