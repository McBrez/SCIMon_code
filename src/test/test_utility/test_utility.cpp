// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

#include <utilities.hpp>

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing utility functions", "[Utilities::split()]") {

  SECTION("Normal string") {
    string testString = "This is a test string!";
    std::vector<string> targetResult = {"This", "is", "a", "test", "string!"};

    unsigned char separator = ' ';
    std::vector<string> result = Utilities::split(testString, separator);

    REQUIRE(targetResult == result);
  }

  SECTION("String without tokens") {
    string testString = "ThisIsAContiguousString!";
    std::vector<string> targetResult = {"ThisIsAContiguousString!"};

    unsigned char separator = ' ';
    std::vector<string> result = Utilities::split(testString, separator);

    REQUIRE(targetResult == result);
  }

  SECTION("Empty string") {
    string testString = "";
    std::vector<string> targetResult = {""};

    unsigned char separator = ' ';
    std::vector<string> result = Utilities::split(testString, separator);

    REQUIRE(targetResult == result);
  }
}
