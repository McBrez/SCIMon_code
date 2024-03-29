#include <fstream>

// 3rd party includes
#include <argparse/argparse.hpp>
#include <easylogging++.h>

// Project includes
#include "data_manager_hdf.hpp"
#include "utilities.hpp"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[]) {
  LOG(INFO) << "Starting up extract_tool";

  argparse::ArgumentParser program("extract_tool");
  program.add_description(
      "This tool takes HDF files, that have been generated by the SCIMon "
      "software, and converts the contained data into other formats. \n\n "
      "Example: \n extract_tool --output-format CSV --impedance-format polar "
      "\"C:/Users/Foo/file.hdf\"");
  program.add_argument("input-file").help("Path to the input HDF file.");
  program.add_argument("-f", "--output-format")
      .default_value(std::string{"CSV"})
      .choices("CSV")
      .help("The type of the output format. Allowed options: CSV");
  program.add_argument("--csv-separator")
      .help("The CSV separator, when \"--output-format CSV\" is given. ")
      .default_value(",");
  program.add_argument("--impedance-format")
      .help("The format impedances shall appear in the output file. Allowed "
            "options: cartesian, polar")
      .default_value(std::string{"cartesian"})
      .choices("cartesian", "polar");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  auto inputFile = program.get<std::string>("input-file");

  LOG(INFO) << "Trying to open " << inputFile << ".";

  Utilities::DataManagerHdf dataManager;
  if (!dataManager.open(inputFile)) {
    LOG(ERROR) << "Could not open" << inputFile << ".";
    return 1;
  }

  if ("CSV" == program.get<std::string>("output-format")) {
    std::map<std::string, std::stringstream *> ss;
    dataManager.writeToCsv(ss, program.get<std::string>("csv-separator")[0],
                           program.get<std::string>("impedance-format"));
    for (auto stream : ss) {
      auto nameSplit = Utilities::split(stream.first, '/');
      std::string fileName = nameSplit.back() + ".csv";
      LOG(INFO) << "Writing to " << fileName;
      std::ofstream fileStream(fileName);
      fileStream << stream.second->str();
      fileStream.close();
    }

    LOG(INFO) << "Wrote to " << ss.size() << " files.";
  } else {
    LOG(ERROR) << "Invalid output format.";
    return 1;
  }

  LOG(INFO) << "Finished. Bye.";

  return 0;
}
