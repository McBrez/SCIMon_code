// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <device_isx3.hpp>

using namespace Devices;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing the implementation of the Sciospec ISX3 device",
          "[Devices]") {
  DeviceIsx3 dut;
  dut.write(shared_ptr<InitDeviceMessage>());
}
