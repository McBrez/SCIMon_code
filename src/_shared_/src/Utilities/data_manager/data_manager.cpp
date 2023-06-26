// Project includes
#include <data_manager.hpp>
#include <data_manager_hdf.hpp>

using namespace Utilities;

DataManager::~DataManager() {}

bool DataManager::isOpen() const { return this->openFlag; }

KeyMapping DataManager::getKeyMapping() const { return this->typeMapping; }

std::shared_ptr<DataManager>
DataManager::getDataManager(DataManagerType dataManagerType) {
  if (DataManagerType::DATAMANAGER_TYPE_HDF == dataManagerType) {
    return std::shared_ptr<DataManager>(new DataManagerHdf());
  } else {
    return std::shared_ptr<DataManager>();
  }
}
