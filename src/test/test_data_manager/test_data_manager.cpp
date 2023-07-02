// Standard includes
#include <chrono>
#include <cstdio>
#include <memory>

// 3rd party includes
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <data_manager_hdf.hpp>

INITIALIZE_EASYLOGGINGPP

#define TEST_FILE_NAME "test_file.hdf"

using namespace Utilities;

TEST_CASE("Opening HDF data manager") {
  std::remove(TEST_FILE_NAME);

  std::shared_ptr<DataManager> dut =
      std::shared_ptr<DataManager>(new DataManagerHdf());

  KeyMapping keyMapping;
  keyMapping["int"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_INT;
  keyMapping["double"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_DOUBLE;
  keyMapping["string"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_STRING;
  keyMapping["complex"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_COMPLEX;

  REQUIRE(dut->open("test_file.hdf", keyMapping));
  REQUIRE(dut->getKeyMapping() ==
          KeyMapping{{"int", DATAMANAGER_DATA_TYPE_INT},
                     {"double", DATAMANAGER_DATA_TYPE_DOUBLE},
                     {"string", DATAMANAGER_DATA_TYPE_STRING},
                     {"complex", DATAMANAGER_DATA_TYPE_COMPLEX}});

  dut.reset();

  // Reopen file.
  dut.reset(new DataManagerHdf());
  REQUIRE(dut->open("test_file.hdf"));
  REQUIRE(dut->getKeyMapping() ==
          KeyMapping{{"int", DATAMANAGER_DATA_TYPE_INT},
                     {"double", DATAMANAGER_DATA_TYPE_DOUBLE},
                     {"string", DATAMANAGER_DATA_TYPE_STRING},
                     {"complex", DATAMANAGER_DATA_TYPE_COMPLEX}});
}

TEST_CASE("Test creating new indices of the HDF data manager") {
  std::remove(TEST_FILE_NAME);

  std::shared_ptr<DataManager> dut =
      std::shared_ptr<DataManager>(new DataManagerHdf());

  // Empty key mapping
  KeyMapping keyMapping;

  REQUIRE(dut->open("test_file.hdf", keyMapping));
  REQUIRE(dut->getKeyMapping().size() == 0);

  // Create new key.
  REQUIRE(dut->createKey("testKey",
                         DataManagerDataType::DATAMANAGER_DATA_TYPE_INT));
  REQUIRE(
      dut->getKeyMapping() ==
      KeyMapping{{"testKey", DataManagerDataType::DATAMANAGER_DATA_TYPE_INT}});

  // Try to write to the new key.
  TimePoint now = getNow();
  REQUIRE(dut->write(now, "testKey", Value(1)));

  // Try to read from the new key.
  Value readBack;
  REQUIRE(dut->read(now, "testKey", readBack));
  REQUIRE(readBack == Value(1));

  // Add another key.
  // Create new key.
  REQUIRE(dut->createKey("anotherTestKey",
                         DataManagerDataType::DATAMANAGER_DATA_TYPE_DOUBLE));
  REQUIRE(
      dut->getKeyMapping() ==
      KeyMapping{{"testKey", DataManagerDataType::DATAMANAGER_DATA_TYPE_INT},
                 {"anotherTestKey",
                  DataManagerDataType::DATAMANAGER_DATA_TYPE_DOUBLE}});
}

TEST_CASE("Test reads and writes of the HDF data manager") {

  std::remove(TEST_FILE_NAME);

  std::shared_ptr<DataManager> dut =
      std::shared_ptr<DataManager>(new DataManagerHdf());

  KeyMapping keyMapping;
  keyMapping["int"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_INT;
  keyMapping["double"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_DOUBLE;
  keyMapping["string"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_STRING;
  keyMapping["complex"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_COMPLEX;
  keyMapping["spectrum"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_SPECTRUM;

  std::vector<double> testFrequencies{1.0,     10.0,     100.0,    1000.0,
                                      10000.0, 100000.0, 1000000.0};
  std::vector<Impedance> testImpedances{{1.0, 2.0},  {3.0, 4.0},  {5.0, 6.0},
                                        {7.0, 8.0},  {9.0, 10.0}, {11.0, 12.0},
                                        {13.0, 14.0}};
  ImpedanceSpectrum testSpectrum;
  Utilities::joinImpedanceSpectrum(testFrequencies, testImpedances,
                                   testSpectrum);
  dut->setupSpectrum("spectrum", testFrequencies);
  REQUIRE(dut->open("test_file.hdf", keyMapping));

  TimePoint now = getNow();
  TimePoint beforeNow = now - std::chrono::milliseconds(2);
  TimePoint afterNow = now + std::chrono::milliseconds(2);

  REQUIRE(dut->write(now, "int", Value(1)));
  REQUIRE(dut->write(now, "double", Value(1.2)));
  REQUIRE(dut->write(now, "string", Value("test")));
  REQUIRE(dut->write(now, "complex", Value(Impedance(1.0, 2.0))));
  REQUIRE(dut->write(now, "spectrum", Value()));

  Value readValueInt;
  Value readValueDouble;
  Value readValueString;
  Value readValueComplex;
  // Try to read at the exact timestamp.
  REQUIRE(dut->read(now, "int", readValueInt));
  REQUIRE(dut->read(now, "double", readValueDouble));
  REQUIRE(dut->read(now, "string", readValueString));
  REQUIRE(dut->read(now, "complex", readValueComplex));
  REQUIRE(std::get<int>(readValueInt) == 1);
  REQUIRE(std::get<double>(readValueDouble) == 1.2);
  REQUIRE(std::get<std::string>(readValueString) == "test");
  REQUIRE(std::get<Impedance>(readValueComplex) == Impedance(1.0, 2.0));
  readValueInt = 0;
  readValueDouble = 0.0;
  readValueString = "";
  readValueComplex = Impedance(0.0, 0.0);
  // Try to read before the exact timestamp.
  REQUIRE(!dut->read(beforeNow, "int", readValueInt));
  REQUIRE(!dut->read(beforeNow, "double", readValueDouble));
  REQUIRE(!dut->read(beforeNow, "string", readValueString));
  REQUIRE(!dut->read(beforeNow, "complex", readValueComplex));
  readValueInt = 0;
  readValueDouble = 0.0;
  readValueString = "";
  readValueComplex = Impedance(0.0, 0.0);
  // Try to read after the exact timestamp.
  REQUIRE(!dut->read(afterNow, "int", readValueInt));
  REQUIRE(!dut->read(afterNow, "double", readValueDouble));
  REQUIRE(!dut->read(afterNow, "string", readValueString));
  REQUIRE(!dut->read(afterNow, "complex", readValueComplex));
  readValueInt = 0;
  readValueDouble = 0.0;
  readValueString = "";
  readValueComplex = Impedance(0.0, 0.0);
  // Try to read time ranges.
  std::vector<Value> readValueVec;
  std::vector<TimePoint> readTimestampVec;
  // int
  REQUIRE(
      dut->read(beforeNow, afterNow, "int", readTimestampVec, readValueVec));
  REQUIRE(readValueVec == std::vector<Value>{1});
  REQUIRE(readTimestampVec == std::vector<TimePoint>{now});
  readValueVec.clear();
  readTimestampVec.clear();
  // double
  REQUIRE(
      dut->read(beforeNow, afterNow, "double", readTimestampVec, readValueVec));
  REQUIRE(readValueVec == std::vector<Value>{1.2});
  REQUIRE(readTimestampVec == std::vector<TimePoint>{now});
  readValueVec.clear();
  readTimestampVec.clear();
  // string
  REQUIRE(
      dut->read(beforeNow, afterNow, "string", readTimestampVec, readValueVec));
  REQUIRE(readValueVec == std::vector<Value>{"test"});
  REQUIRE(readTimestampVec == std::vector<TimePoint>{now});
  readValueVec.clear();
  readTimestampVec.clear();
  // complex
  REQUIRE(dut->read(beforeNow, afterNow, "complex", readTimestampVec,
                    readValueVec));
  REQUIRE(readValueVec == std::vector<Value>{Impedance(1.0, 2.0)});
  REQUIRE(readTimestampVec == std::vector<TimePoint>{now});
  readValueVec.clear();
  readTimestampVec.clear();

  // Try to read before the time ranges.
  TimePoint longBeforeNow = now - std::chrono::milliseconds(4);
  // int
  REQUIRE(dut->read(longBeforeNow, beforeNow, "int", readTimestampVec,
                    readValueVec));
  REQUIRE(readValueVec.size() == 0);
  REQUIRE(readTimestampVec.size() == 0);
  readValueVec.clear();
  readTimestampVec.clear();
  // double
  REQUIRE(dut->read(longBeforeNow, beforeNow, "double", readTimestampVec,
                    readValueVec));
  REQUIRE(readValueVec.size() == 0);
  REQUIRE(readTimestampVec.size() == 0);
  readValueVec.clear();
  readTimestampVec.clear();
  // string
  REQUIRE(dut->read(longBeforeNow, beforeNow, "string", readTimestampVec,
                    readValueVec));
  REQUIRE(readValueVec.size() == 0);
  REQUIRE(readTimestampVec.size() == 0);
  readValueVec.clear();
  readTimestampVec.clear();
  // complex
  REQUIRE(dut->read(longBeforeNow, beforeNow, "complex", readTimestampVec,
                    readValueVec));
  REQUIRE(readValueVec.size() == 0);
  REQUIRE(readTimestampVec.size() == 0);
  readValueVec.clear();
  readTimestampVec.clear();
  // Try to read after the time ranges.
  TimePoint longAfterNow = now + std::chrono::milliseconds(4);
  // int
  REQUIRE(
      dut->read(afterNow, longAfterNow, "int", readTimestampVec, readValueVec));
  REQUIRE(readValueVec.size() == 0);
  REQUIRE(readTimestampVec.size() == 0);
  readValueVec.clear();
  readTimestampVec.clear();
  // double
  REQUIRE(dut->read(afterNow, longAfterNow, "double", readTimestampVec,
                    readValueVec));
  REQUIRE(readValueVec.size() == 0);
  REQUIRE(readTimestampVec.size() == 0);
  readValueVec.clear();
  readTimestampVec.clear();
  // string
  REQUIRE(dut->read(afterNow, longAfterNow, "string", readTimestampVec,
                    readValueVec));
  REQUIRE(readValueVec.size() == 0);
  REQUIRE(readTimestampVec.size() == 0);
  readValueVec.clear();
  readTimestampVec.clear();
  // complex
  REQUIRE(dut->read(afterNow, longAfterNow, "complex", readTimestampVec,
                    readValueVec));
  REQUIRE(readValueVec.size() == 0);
  REQUIRE(readTimestampVec.size() == 0);
  readValueVec.clear();
  readTimestampVec.clear();
}

#define TEST_VECTOR_SIZE 1000000
TEST_CASE("Test mass writes") {
  std::remove(TEST_FILE_NAME);

  std::shared_ptr<DataManager> dut =
      std::shared_ptr<DataManager>(new DataManagerHdf());

  KeyMapping keyMapping;
  keyMapping["int"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_INT;
  keyMapping["double"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_DOUBLE;
  keyMapping["string"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_STRING;
  keyMapping["complex"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_COMPLEX;

  REQUIRE(dut->open("test_file.hdf", keyMapping));

  // Create the vectors that shall be written.
  TimePoint now = getNow();
  std::vector<TimePoint> timestamps;
  timestamps.reserve(TEST_VECTOR_SIZE);
  std::vector<Value> valuesInt;
  valuesInt.reserve(TEST_VECTOR_SIZE);
  for (int i = 0; i < TEST_VECTOR_SIZE; i++) {
    timestamps.push_back(now + std::chrono::milliseconds(i));
    valuesInt.push_back(Value(i));
  }

  REQUIRE(dut->write(timestamps, "int", valuesInt));
  TimePoint from = now;
  TimePoint to = now + std::chrono::milliseconds(1000);

  std::vector<TimePoint> readTimestamps;
  std::vector<Value> readValues;
  REQUIRE(dut->read(from, to, "int", readTimestamps, readValues));
}

TEST_CASE("Mass single writes") {
  std::remove(TEST_FILE_NAME);

  std::shared_ptr<DataManager> dut =
      std::shared_ptr<DataManager>(new DataManagerHdf());

  KeyMapping keyMapping;
  keyMapping["int"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_INT;
  keyMapping["double"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_DOUBLE;
  keyMapping["string"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_STRING;
  keyMapping["complex"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_COMPLEX;

  REQUIRE(dut->open("test_file.hdf", keyMapping));

  BENCHMARK("Single Write INT") {
    return dut->write(Core::getNow(), "int", Value(1));
  };
  BENCHMARK("Single Write DOUBLE") {
    return dut->write(Core::getNow(), "double", Value(1.2));
  };
  BENCHMARK("Single Write string") {
    return dut->write(Core::getNow(), "string", Value("test"));
  };
  BENCHMARK("Single Write complex") {
    return dut->write(Core::getNow(), "complex", Value(Impedance(1.0, 2.0)));
  };
}

TEST_CASE("Single read") {
  std::remove(TEST_FILE_NAME);

  std::shared_ptr<DataManager> dut =
      std::shared_ptr<DataManager>(new DataManagerHdf());

  KeyMapping keyMapping;
  keyMapping["int"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_INT;
  keyMapping["double"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_DOUBLE;
  keyMapping["string"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_STRING;
  keyMapping["complex"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_COMPLEX;

  REQUIRE(dut->open("test_file.hdf", keyMapping));

  TimePoint now = getNow();
  TimePoint beforeNow = now - std::chrono::milliseconds(2);
  TimePoint afterNow = now + std::chrono::milliseconds(2);

  REQUIRE(dut->write(now, "int", Value(1)));
  REQUIRE(dut->write(now, "double", Value(1.2)));
  REQUIRE(dut->write(now, "string", Value("test")));
  REQUIRE(dut->write(now, "complex", Value(Impedance(1.0, 2.0))));

  Value valueInt;
  BENCHMARK("Single Read INT") { return dut->read(now, "int", valueInt); };
  Value valueDouble;
  BENCHMARK("Single Read DOUBLE") {
    return dut->read(now, "double", valueDouble);
  };
  Value valueString;
  BENCHMARK("Single Read string") {
    return dut->read(now, "string", valueString);
  };
  Value valueComplex;
  BENCHMARK("Single Rrite complex") {
    return dut->read(now, "complex", valueComplex);
  };
}
