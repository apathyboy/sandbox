/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SANDBOX_PROGRAM_OPTIONS_H_
#define SANDBOX_PROGRAM_OPTIONS_H_

#include <cstdint>
#include <string>
#include <boost/program_options.hpp>

namespace sandbox {

/** Builds program options from command line and file input sources.
 *
 * @param vm Container to store the program options in
 * @param argc Passed through from main(argc, argv)
 * @param argv Passed through from main(argc, argv)
 * @param filename Name of the file containing program options
 *
 * @return Returns a variables_map containing the program options.
 */
void BuildProgramOptions(boost::program_options::variables_map&, int argc, char *argv[], const std::string& configure);

}  // namespace sandbox

#endif  // SANDBOX_PROGRAM_OPTIONS_H_