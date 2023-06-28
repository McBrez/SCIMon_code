// Standard includes
#include <memory>

// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <data_manager_hdf.hpp>

INITIALIZE_EASYLOGGINGPP

using namespace Utilities;
TEST_CASE("Test the HDF data manager") {

  std::shared_ptr<DataManager> dut =
      std::shared_ptr<DataManager>(new DataManagerHdf());

  KeyMapping keyMapping;
  keyMapping["type_int"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_INT;
  keyMapping["type_double"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_DOUBLE;
  keyMapping["type_string"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_STRING;
  keyMapping["type_complex"] =
      DataManagerDataType::DATAMANAGER_DATA_TYPE_COMPLEX;
  dut->open("test_file.hdf", keyMapping);

  bool openSuccess = dut->open("test_file.hdf", keyMapping);
  REQUIRE(openSuccess);
}
