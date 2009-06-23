/**
 * Legacy Sandbox Project - ProgramOptions.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef PROGRAM_OPTIONS_H_
#define PROGRAM_OPTIONS_H_

#include <string>
#include <boost/program_options.hpp>

/** Builds program options from command line and file input sources.
 *
 * @param vm Container to store the program options in
 * @param argc Passed through from main(argc, argv)
 * @param argv Passed through from main(argc, argv)
 * @param filename Name of the file containing program options
 *
 * @return Returns a variables_map containing the program options.
 */
boost::program_options::variables_map buildProgramOptions(int argc, char *argv[], const std::string& configure);

#endif // PROGRAM_OPTIONS_H_