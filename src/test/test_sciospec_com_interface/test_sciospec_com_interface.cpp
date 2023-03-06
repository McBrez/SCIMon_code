// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

#include <com_interface_codec.hpp>

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing ComInterfaceCodec") {
    Devices::ComInterfaceCodec dut;
    dut.decodeMessage({0x01, 0x02});
}


