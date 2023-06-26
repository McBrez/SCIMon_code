// Project includes
#include <data_manager_hdf.hpp>

using namespace Utilities;

DataManagerHdf::~DataManagerHdf() {this->close();}

bool DataManagerHdf::read(TimePoint timestamp, std::string key,
                                  Value &value) {}


bool DataManagerHdf::read(TimePoint from, TimePoint to, std::string key,
                                  Value &value) {

}


bool DataManagerHdf::write(TimePoint timestamp, std::string key,
                                   const Value &value) {}

bool DataManagerHdf::open(std::string name) {}

bool DataManagerHdf::open(std::string name, KeyMapping keyMapping,
                                  bool force = false) {}

bool DataManagerHdf::close() {
    if(this->isOpen()) {
        return true;
    }
}


DataManagerType getDataManagerType() const {
    return DataManagerType::DATAMANAGER_TYPE_HDF;
}
