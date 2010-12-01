/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/


#include "sandbox/program_options.h"

#include <cstdint>

#include <fstream>
#include <iostream>

#if HAVE_CONFIG_H
#include "build-aux/config.h"
#else
const char * PACKAGE_VERSION = "0.1.1";
#endif

namespace po = boost::program_options;

namespace sandbox {

void BuildProgramOptions(boost::program_options::variables_map& vm,
  int argc, char *argv[], const std::string& filename) {
  // Declare a group of options that will be allowed only on command line
  po::options_description general("General Options");
  general.add_options()
    ("version,v", "print version string")
    ("help,h", "produce help message");

  po::options_description cmdline_options("Command Line Options");
  cmdline_options.add(general);

  po::options_description config_file_options("Configuration File Options");
  config_file_options.add_options()
    ("login_port", po::value<uint16_t>()->default_value(44453),
      "Port the login server will listen on")

    ("ping_port", po::value<uint16_t>()->default_value(44462),
      "Port the ping server will listen on")

    ("zone_port", po::value<uint16_t>()->default_value(44463),
      "Port the zone server will listen on")

    ("shuttle_wait_time", po::value<uint16_t>()->default_value(1),
      "Length of time in minutes between shuttle visits");

  po::options_description visible("Allowed options");
  visible.add(config_file_options).add(cmdline_options);

  po::store(po::parse_command_line(argc, argv, cmdline_options), vm);

  std::ifstream ifs(filename.c_str());
  po::store(po::parse_config_file(ifs, config_file_options),  vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << visible << std::endl;

    exit(EXIT_SUCCESS);
  }

  if (vm.count("version")) {
    std::cout << "SWG:ANH Sandbox Server v" << PACKAGE_VERSION << std::endl;
    std::cout << "Copyright (c) 2010 The SWG:ANH Team" << std::endl;

    exit(EXIT_SUCCESS);
  }
}

}  // namespace sandbox
