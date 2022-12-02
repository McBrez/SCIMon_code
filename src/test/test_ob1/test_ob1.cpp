// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <Elveflow64.h>
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <device_ob1_win.hpp>

using namespace Devices;
using namespace Messages;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing the implementation of the ElveFlow OB1 device",
          "[Devices]") {
  // Build up
  DeviceOb1 *dut;
#ifdef WIN32
  dut = new DeviceOb1Win();
#else
#error Target operating systems other than WIN32 are not yet supported.
#endif

  SECTION("Initializing") {

    shared_ptr<InitDeviceMessage> initMsg(new InitDeviceMessage());
    InitPayload *payload = new InitPayload();
    payload->deviceName = "01F8E63F";
    payload->channelOneConnfig = Z_regulator_type__0_2000_mbar;
    payload->channelTwoConnfig = Z_regulator_type__0_2000_mbar;
    payload->channelThreeConnfig = Z_regulator_type_none;
    payload->channelFourConnfig = Z_regulator_type_none;
    initMsg->setPayload(payload);
    REQUIRE(dut->write(initMsg) == true);
    auto ackMsg = dut->read();
    REQUIRE(!ackMsg);
  }

  // Tear down.
  delete dut;
}
