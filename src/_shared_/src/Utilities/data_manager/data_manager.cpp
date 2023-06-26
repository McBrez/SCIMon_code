// Project includes
#include <data_manager.hpp>
#include <data_manager_hdf.hpp>

using namespace Utilities;

DataManager::~DataManager() {}

bool DataManager::isOpen() { return this->openFlag; }

KeyMapping DataManager::getKeyMapping() { return this->keyMapping; }

std::shared_ptr<DataManager> getDataManager(DataManagerType dataManagerType) {
    if(DataManagerType::DATAMANAGER_TYPE_HDF == dataManagerType) {
        return std::shared_ptr<DataManager>(new DataManagerHdf());
    }
}
