/*
 * OpenSWG Combat Upgrade Sandbox - ProgramOptions.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include <fstream>
#include <iostream>

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#if HAVE_CONFIG_H
#	include <config.h>
#else
const char * PACKAGE_VERSION = "1.1.0";
#endif

#include "ProgramOptions.h"

namespace po = boost::program_options;

void buildProgramOptions(boost::program_options::variables_map& vm, int argc, char *argv[], const std::string& filename)
{	
    // Declare a group of options that will be allowed only on command line
    po::options_description general("General Options");
    general.add_options()
        ("version,v", "print version string")
        ("help,h", "produce help message");
	
    po::options_description cmdline_options("Command Line Options");
    cmdline_options.add(general);
	
    po::options_description config_file_options("Configuration File Options");
    config_file_options.add_options()
        ("login_port", po::value<uint16_t>()->default_value(44453), "Port the login server will listen on")
        ("ping_port",  po::value<uint16_t>()->default_value(44462), "Port the ping server will listen on")
        ("zone_port",  po::value<uint16_t>()->default_value(44463), "Port the zone server will listen on")
        ("shuttle_wait_time",  po::value<uint16_t>()->default_value(1), "Length of time in minutes between shuttle visits")
    ;
    
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
        std::cout << "OpenSWG Legacy Sandbox (Revisited) " << PACKAGE_VERSION << std::endl;
        std::cout << "Copyright (c) 2009 Eric S. Barr Jr. <eric.barr@openswg.com>" << std::endl;
  
        exit(EXIT_SUCCESS);
    }
}