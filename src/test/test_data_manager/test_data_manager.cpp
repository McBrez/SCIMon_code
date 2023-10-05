// Standard includes
#include <chrono>
#include <cstdio>
#include <memory>
#include <sstream>

// 3rd party includes
#define CATCH_CONFIG_MAIN
// #define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <data_manager_hdf.hpp>

INITIALIZE_EASYLOGGINGPP

const std::string TestFileName = "test_file";
const std::string TestFileNameExt = TestFileName + ".hdf";

using namespace Utilities;

std::ostream &operator<<(std::ostream &out, std::vector<Value> const &t) {
  if (t.empty()) {
    return out;
  }

  for (auto &it : t) {
    if (std::holds_alternative<Impedance>(it)) {
      Impedance impedance = std::get<Impedance>(it);
      out << ",{" << std::to_string(impedance.real()) << ", "
          << std::to_string(impedance.imag()) << "}";
    }
  }

  return out;
}

TEST_CASE("Opening HDF data manager") {

  std::remove(TestFileNameExt.c_str());

  std::shared_ptr<DataManager> dut =
      std::shared_ptr<DataManager>(new DataManagerHdf());

  KeyMapping keyMapping;
  keyMapping["int"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_INT;
  keyMapping["double"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_DOUBLE;
  keyMapping["string"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_STRING;
  keyMapping["complex"] = DataManagerDataType::DATAMANAGER_DATA_TYPE_COMPLEX;

  REQUIRE(dut->open(TestFileName, keyMapping));
  REQUIRE(dut->getKeyMapping() ==
          KeyMapping{{"int", DATAMANAGER_DATA_TYPE_INT},
                     {"double", DATAMANAGER_DATA_TYPE_DOUBLE},
                     {"string", DATAMANAGER_DATA_TYPE_STRING},
                     {"complex", DATAMANAGER_DATA_TYPE_COMPLEX}});

  dut.reset();

  // Reopen file.
  dut.reset(new DataManagerHdf());
  REQUIRE(dut->open(TestFileNameExt));
  REQUIRE(dut->getKeyMapping() ==
          KeyMapping{{"int", DATAMANAGER_DATA_TYPE_INT},
                     {"double", DATAMANAGER_DATA_TYPE_DOUBLE},
                     {"string", DATAMANAGER_DATA_TYPE_STRING},
                     {"complex", DATAMANAGER_DATA_TYPE_COMPLEX}});
}

TEST_CASE("Test creating new indices of the HDF data manager") {
  std::remove(TestFileNameExt.c_str());

  std::shared_ptr<DataManager> dut =
      std::shared_ptr<DataManager>(new DataManagerHdf());

  // Empty key mapping
  KeyMapping keyMapping;

  REQUIRE(dut->open(TestFileName, keyMapping));
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

TEST_CASE("Test single reads and writes of the HDF data manager") {

  std::remove(TestFileNameExt.c_str());

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
  REQUIRE(dut->open(TestFileName, keyMapping));
  dut->setupSpectrum("spectrum", testFrequencies);

  TimePoint now = getNow();
  TimePoint beforeNow = now - std::chrono::milliseconds(2);
  TimePoint afterNow = now + std::chrono::milliseconds(2);

  REQUIRE(dut->write(now, "int", Value(1)));
  REQUIRE(dut->write(now, "double", Value(1.2)));
  REQUIRE(dut->write(now, "string", Value("test")));
  REQUIRE(dut->write(now, "complex", Value(Impedance(1.0, 2.0))));
  REQUIRE(dut->write(now, "spectrum", Value(testSpectrum)));

  Value readValueInt;
  Value readValueDouble;
  Value readValueString;
  Value readValueComplex;
  Value readValueSpectrum;
  // Try to read at the exact timestamp.
  REQUIRE(dut->read(now, "int", readValueInt));
  REQUIRE(dut->read(now, "double", readValueDouble));
  REQUIRE(dut->read(now, "string", readValueString));
  REQUIRE(dut->read(now, "complex", readValueComplex));
  REQUIRE(dut->read(now, "spectrum", readValueSpectrum));
  REQUIRE(std::get<int>(readValueInt) == 1);
  REQUIRE(std::get<double>(readValueDouble) == 1.2);
  REQUIRE(std::get<std::string>(readValueString) == "test");
  REQUIRE(std::get<Impedance>(readValueComplex) == Impedance(1.0, 2.0));
  REQUIRE(std::get<ImpedanceSpectrum>(readValueSpectrum) == testSpectrum);
  readValueInt = 0;
  readValueDouble = 0.0;
  readValueString = "";
  readValueComplex = Impedance(0.0, 0.0);
  readValueSpectrum = ImpedanceSpectrum();
  // Try to read before the exact timestamp.
  REQUIRE(!dut->read(beforeNow, "int", readValueInt));
  REQUIRE(!dut->read(beforeNow, "double", readValueDouble));
  REQUIRE(!dut->read(beforeNow, "string", readValueString));
  REQUIRE(!dut->read(beforeNow, "complex", readValueComplex));
  REQUIRE(!dut->read(beforeNow, "spectrum", readValueSpectrum));
  readValueInt = 0;
  readValueDouble = 0.0;
  readValueString = "";
  readValueComplex = Impedance(0.0, 0.0);
  readValueSpectrum = ImpedanceSpectrum();
  // Try to read after the exact timestamp.
  REQUIRE(!dut->read(afterNow, "int", readValueInt));
  REQUIRE(!dut->read(afterNow, "double", readValueDouble));
  REQUIRE(!dut->read(afterNow, "string", readValueString));
  REQUIRE(!dut->read(afterNow, "complex", readValueComplex));
  REQUIRE(!dut->read(afterNow, "spectrum", readValueComplex));
  readValueInt = 0;
  readValueDouble = 0.0;
  readValueString = "";
  readValueComplex = Impedance(0.0, 0.0);
  readValueSpectrum = ImpedanceSpectrum();
}

TEST_CASE("Test vectorized reads and writes of the HDF data manager") {

  std::remove(TestFileNameExt.c_str());

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

  REQUIRE(dut->open(TestFileName, keyMapping));
  dut->setupSpectrum("spectrum", testFrequencies);

  TimePoint now = getNow();
  TimePoint beforeNow = now - std::chrono::minutes(1);
  TimePoint afterNow = now + std::chrono::minutes(1);

  // Build the vectors that shall be written.
  std::vector<Value> valueVectorInt = std::vector<Value>(120, Value(1));
  std::vector<Value> valueVectorDouble = std::vector<Value>(120, Value(1.2));
  std::vector<Value> valueVectorString = std::vector<Value>(120, Value("test"));
  std::vector<Value> valueVectorComplex =
      std::vector<Value>(120, Value(Impedance(1.0, 2.0)));
  std::vector<Value> valueVectorSpectrum =
      std::vector<Value>(120, Value(testSpectrum));

  std::vector<TimePoint> timePointVector;
  timePointVector.reserve(120);
  for (int i = 0; i < 120; i++) {
    timePointVector.emplace_back(beforeNow + std::chrono::seconds(i));
  }

  REQUIRE(dut->write(timePointVector, "int", valueVectorInt));
  REQUIRE(dut->write(timePointVector, "double", valueVectorDouble));
  REQUIRE(dut->write(timePointVector, "string", valueVectorString));
  REQUIRE(dut->write(timePointVector, "complex", valueVectorComplex));
  REQUIRE(dut->write(timePointVector, "spectrum", valueVectorSpectrum));

  std::vector<Value> readValueInt;
  std::vector<TimePoint> readTimestampsInt;
  std::vector<Value> readValueDouble;
  std::vector<TimePoint> readTimestampsDouble;
  std::vector<Value> readValueString;
  std::vector<TimePoint> readTimestampsString;
  std::vector<Value> readValueComplex;
  std::vector<TimePoint> readTimestampsComplex;
  std::vector<Value> readValueSpectrum;
  std::vector<TimePoint> readTimestampsSpectrum;

  // Try to read the exact range.
  REQUIRE(
      dut->read(beforeNow, afterNow, "int", readTimestampsInt, readValueInt));
  REQUIRE(dut->read(beforeNow, afterNow, "double", readTimestampsDouble,
                    readValueDouble));
  REQUIRE(dut->read(beforeNow, afterNow, "string", readTimestampsString,
                    readValueString));
  REQUIRE(dut->read(beforeNow, afterNow, "complex", readTimestampsComplex,
                    readValueComplex));
  REQUIRE(dut->read(beforeNow, afterNow, "spectrum", readTimestampsSpectrum,
                    readValueSpectrum));

  REQUIRE(valueVectorInt == readValueInt);
  REQUIRE(valueVectorDouble == readValueDouble);
  REQUIRE(valueVectorString == readValueString);
  REQUIRE(valueVectorComplex == readValueComplex);
  REQUIRE(valueVectorSpectrum == readValueSpectrum);
  REQUIRE(timePointVector == readTimestampsInt);
  REQUIRE(timePointVector == readTimestampsDouble);
  REQUIRE(timePointVector == readTimestampsString);
  REQUIRE(timePointVector == readTimestampsComplex);
  REQUIRE(timePointVector == readTimestampsSpectrum);

  // Try to read before the range.
  readValueInt.clear();
  readTimestampsInt.clear();
  readValueDouble.clear();
  readTimestampsDouble.clear();
  readValueString.clear();
  readTimestampsString.clear();
  readValueComplex.clear();
  readTimestampsComplex.clear();
  readValueSpectrum.clear();
  readTimestampsSpectrum.clear();

  TimePoint veryLongBeforeNow = beforeNow - std::chrono::minutes(2);
  TimePoint longBeforeNow = beforeNow - std::chrono::minutes(1);

  REQUIRE(dut->read(veryLongBeforeNow, longBeforeNow, "int", readTimestampsInt,
                    readValueInt));
  REQUIRE(dut->read(veryLongBeforeNow, longBeforeNow, "double",
                    readTimestampsDouble, readValueDouble));
  REQUIRE(dut->read(veryLongBeforeNow, longBeforeNow, "string",
                    readTimestampsString, readValueString));
  REQUIRE(dut->read(veryLongBeforeNow, longBeforeNow, "complex",
                    readTimestampsComplex, readValueComplex));
  REQUIRE(dut->read(veryLongBeforeNow, longBeforeNow, "spectrum",
                    readTimestampsSpectrum, readValueSpectrum));

  REQUIRE(readValueInt == std::vector<Value>{});
  REQUIRE(readValueDouble == std::vector<Value>{});
  REQUIRE(readValueString == std::vector<Value>{});
  REQUIRE(readValueComplex == std::vector<Value>{});
  REQUIRE(readValueSpectrum == std::vector<Value>{});
  REQUIRE(readTimestampsInt == std::vector<TimePoint>{});
  REQUIRE(readTimestampsDouble == std::vector<TimePoint>{});
  REQUIRE(readTimestampsString == std::vector<TimePoint>{});
  REQUIRE(readTimestampsComplex == std::vector<TimePoint>{});
  REQUIRE(readTimestampsSpectrum == std::vector<TimePoint>{});

  // Try to read a range that intersects from lower timestamps.
  readValueInt.clear();
  readTimestampsInt.clear();
  readValueDouble.clear();
  readTimestampsDouble.clear();
  readValueString.clear();
  readTimestampsString.clear();
  readValueComplex.clear();
  readTimestampsComplex.clear();
  readValueSpectrum.clear();
  readTimestampsSpectrum.clear();

  REQUIRE(
      dut->read(longBeforeNow, now, "int", readTimestampsInt, readValueInt));
  REQUIRE(dut->read(longBeforeNow, now, "double", readTimestampsDouble,
                    readValueDouble));
  REQUIRE(dut->read(longBeforeNow, now, "string", readTimestampsString,
                    readValueString));
  REQUIRE(dut->read(longBeforeNow, now, "complex", readTimestampsComplex,
                    readValueComplex));
  REQUIRE(dut->read(longBeforeNow, now, "spectrum", readTimestampsSpectrum,
                    readValueSpectrum));

  REQUIRE(readValueInt == std::vector<Value>(valueVectorInt.begin(),
                                             valueVectorInt.begin() + 61));
  REQUIRE(readValueDouble ==
          std::vector<Value>(valueVectorDouble.begin(),
                             valueVectorDouble.begin() + 61));
  REQUIRE(readValueString ==
          std::vector<Value>(valueVectorString.begin(),
                             valueVectorString.begin() + 61));
  REQUIRE(readValueComplex ==
          std::vector<Value>(valueVectorComplex.begin(),
                             valueVectorComplex.begin() + 61));
  REQUIRE(readValueSpectrum ==
          std::vector<Value>(valueVectorSpectrum.begin(),
                             valueVectorSpectrum.begin() + 61));
  REQUIRE(readTimestampsInt ==
          std::vector<TimePoint>(timePointVector.begin(),
                                 timePointVector.begin() + 61));
  REQUIRE(readTimestampsDouble ==
          std::vector<TimePoint>(timePointVector.begin(),
                                 timePointVector.begin() + 61));
  REQUIRE(readTimestampsString ==
          std::vector<TimePoint>(timePointVector.begin(),
                                 timePointVector.begin() + 61));
  REQUIRE(readTimestampsComplex ==
          std::vector<TimePoint>(timePointVector.begin(),
                                 timePointVector.begin() + 61));
  REQUIRE(readTimestampsSpectrum ==
          std::vector<TimePoint>(timePointVector.begin(),
                                 timePointVector.begin() + 61));

  // Try to read a range that intersects from higher timestamps.
  readValueInt.clear();
  readTimestampsInt.clear();
  readValueDouble.clear();
  readTimestampsDouble.clear();
  readValueString.clear();
  readTimestampsString.clear();
  readValueComplex.clear();
  readTimestampsComplex.clear();
  readValueSpectrum.clear();
  readTimestampsSpectrum.clear();

  TimePoint longAfterNow = afterNow + std::chrono::minutes(1);

  REQUIRE(dut->read(now, longAfterNow, "int", readTimestampsInt, readValueInt));
  REQUIRE(dut->read(now, longAfterNow, "double", readTimestampsDouble,
                    readValueDouble));
  REQUIRE(dut->read(now, longAfterNow, "string", readTimestampsString,
                    readValueString));
  REQUIRE(dut->read(now, longAfterNow, "complex", readTimestampsComplex,
                    readValueComplex));
  REQUIRE(dut->read(now, longAfterNow, "spectrum", readTimestampsSpectrum,
                    readValueSpectrum));

  REQUIRE(readValueInt == std::vector<Value>(valueVectorInt.begin() + 60,
                                             valueVectorInt.end()));
  REQUIRE(readValueDouble == std::vector<Value>(valueVectorDouble.begin() + 60,
                                                valueVectorDouble.end()));
  REQUIRE(readValueString == std::vector<Value>(valueVectorString.begin() + 60,
                                                valueVectorString.end()));
  REQUIRE(readValueComplex ==
          std::vector<Value>(valueVectorComplex.begin() + 60,
                             valueVectorComplex.end()));
  REQUIRE(readValueSpectrum ==
          std::vector<Value>(valueVectorSpectrum.begin() + 60,
                             valueVectorSpectrum.end()));
  REQUIRE(readTimestampsInt ==
          std::vector<TimePoint>(timePointVector.begin() + 60,
                                 timePointVector.end()));
  REQUIRE(readTimestampsDouble ==
          std::vector<TimePoint>(timePointVector.begin() + 60,
                                 timePointVector.end()));
  REQUIRE(readTimestampsString ==
          std::vector<TimePoint>(timePointVector.begin() + 60,
                                 timePointVector.end()));
  REQUIRE(readTimestampsComplex ==
          std::vector<TimePoint>(timePointVector.begin() + 60,
                                 timePointVector.end()));
  REQUIRE(readTimestampsSpectrum ==
          std::vector<TimePoint>(timePointVector.begin() + 60,
                                 timePointVector.end()));

  // Try to read after the range.
  readValueInt.clear();
  readTimestampsInt.clear();
  readValueDouble.clear();
  readTimestampsDouble.clear();
  readValueString.clear();
  readTimestampsString.clear();
  readValueComplex.clear();
  readTimestampsComplex.clear();
  readValueSpectrum.clear();
  readTimestampsSpectrum.clear();

  TimePoint veryLongAfterNow = afterNow + std::chrono::minutes(2);

  REQUIRE(dut->read(longAfterNow, veryLongAfterNow, "int", readTimestampsInt,
                    readValueInt));
  REQUIRE(dut->read(longAfterNow, veryLongAfterNow, "double",
                    readTimestampsDouble, readValueDouble));
  REQUIRE(dut->read(longAfterNow, veryLongAfterNow, "string",
                    readTimestampsString, readValueString));
  REQUIRE(dut->read(longAfterNow, veryLongAfterNow, "complex",
                    readTimestampsComplex, readValueComplex));
  REQUIRE(dut->read(longAfterNow, veryLongAfterNow, "spectrum",
                    readTimestampsSpectrum, readValueSpectrum));

  REQUIRE(readValueInt == std::vector<Value>{});
  REQUIRE(readValueDouble == std::vector<Value>{});
  REQUIRE(readValueString == std::vector<Value>{});
  REQUIRE(readValueComplex == std::vector<Value>{});
  REQUIRE(readValueSpectrum == std::vector<Value>{});
  REQUIRE(readTimestampsInt == std::vector<TimePoint>{});
  REQUIRE(readTimestampsDouble == std::vector<TimePoint>{});
  REQUIRE(readTimestampsString == std::vector<TimePoint>{});
  REQUIRE(readTimestampsComplex == std::vector<TimePoint>{});
  REQUIRE(readTimestampsSpectrum == std::vector<TimePoint>{});
}

#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING

#define TEST_VECTOR_SIZE 2000
TEST_CASE("Test mass writes") {
  std::remove(TestFileNameExt.c_str());

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

  REQUIRE(dut->open(TestFileName, keyMapping));

  dut->setupSpectrum("spectrum", testFrequencies);

  // Create the vectors that shall be written.
  TimePoint now = getNow();
  std::vector<TimePoint> timestamps;
  timestamps.reserve(TEST_VECTOR_SIZE);
  std::vector<Value> valuesInt;
  valuesInt.reserve(TEST_VECTOR_SIZE);
  std::vector<Value> valuesSpectrum;
  valuesSpectrum.reserve(TEST_VECTOR_SIZE);
  for (int i = 0; i < TEST_VECTOR_SIZE; i++) {
    timestamps.push_back(now + std::chrono::milliseconds(i));
    valuesInt.push_back(Value(i));
    valuesSpectrum.push_back(Value(testSpectrum));
  }

  REQUIRE(dut->write(timestamps, "int", valuesInt));
  REQUIRE(dut->write(timestamps, "spectrum", valuesSpectrum));
  TimePoint from = now;
  TimePoint to = now + std::chrono::milliseconds(1000);

  // Build the reference vectors.
  TimePoint tp = timestamps[0];
  size_t i = 0;
  std::vector<Value> valuesIntRef;
  valuesIntRef.reserve(1000);
  std::vector<Value> valuesSpectrumRef;
  valuesSpectrumRef.reserve(1000);
  while (tp <= to) {
    valuesIntRef.push_back(i);
    valuesSpectrumRef.push_back(testSpectrum);
    i++;
    tp = timestamps[i];
  }

  std::vector<TimePoint> readTimestamps;
  std::vector<Value> readValues;
  REQUIRE(dut->read(from, to, "int", readTimestamps, readValues));
  REQUIRE(dut->read(from, to, "spectrum", readTimestamps, readValues));
}

TEST_CASE("Benchmark single writes") {
  std::remove(TestFileNameExt.c_str());

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

  REQUIRE(dut->open(TestFileName, keyMapping));

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
  BENCHMARK("Single Write spectrum") {
    return dut->write(Core::getNow(), "spectrum", Value(testSpectrum));
  };
}

TEST_CASE("Benchmark single read") {
  std::remove(TestFileNameExt.c_str());

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

  REQUIRE(dut->open("test_file.hdf", keyMapping));
  dut->setupSpectrum("spectrum", testFrequencies);

  TimePoint now = getNow();
  TimePoint beforeNow = now - std::chrono::milliseconds(2);
  TimePoint afterNow = now + std::chrono::milliseconds(2);

  REQUIRE(dut->write(now, "int", Value(1)));
  REQUIRE(dut->write(now, "double", Value(1.2)));
  REQUIRE(dut->write(now, "string", Value("test")));
  REQUIRE(dut->write(now, "complex", Value(Impedance(1.0, 2.0))));
  REQUIRE(dut->write(now, "spectrum", Value(testSpectrum)));

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
  BENCHMARK("Single Read complex") {
    return dut->read(now, "complex", valueComplex);
  };
  Value valueSpectrum;
  BENCHMARK("Single Read spectrum") {
    return dut->read(now, "spectrum", valueSpectrum);
  };
}
#endif
