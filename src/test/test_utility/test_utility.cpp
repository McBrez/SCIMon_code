// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

#include <utility.hpp>

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing utility functions", "[Devices]") {

  SECTION("split") {
    string testString = "This is a test string!";
    std::vector<string> targetResult = {"This", "is", "a", "test", "string!"};

    char separator = ' ';
    std::vector<string> result = Utility::split(testString, separator);

    REQUIRE(targetResult == result);
  }
}
