/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include "Logger.h"
#include "LoginServer.h"
#include "ZoneServer.h"
#include "ProgramOptions.h"

#include <boost/thread.hpp>

#include <exception>
#include <iostream>
#include <string>

/* Program Constants */
const char* config_filename = "cu-sandbox.cfg";

// Run the SWG:ANH Sandbox Server.
int main(int argc, char *argv[]) 
{         
	// Send server header output.
	std::cout << "SWG:ANH Combat Upgrade Sandbox v2.0" << std::endl;
	std::cout << "Copyright (c) 2010 The SWG:ANH Team" << std::endl << std::endl;

    // Load program options and print program info (if requested).
    boost::program_options::variables_map sandbox_options;
    buildProgramOptions(sandbox_options, argc, argv, config_filename);
    
    Logger().log(INFO) << "Server starting with configuration file: " << config_filename;

	// Begin the main execution phase. This begins by starting the server
	// threads and then running until the exit command is received.
    try {		
        LoginServer login_server(sandbox_options["login_port"].as<uint16_t>());
        boost::thread login_thread(std::tr1::bind(&LoginServer::run, &login_server));

        ZoneServer zone_server(sandbox_options["zone_port"].as<uint16_t>());
        boost::thread zone_thread(std::tr1::bind(&ZoneServer::run, &zone_server));

        for(;;) {
            std::string cmd;
            std::cin >> cmd;

            // Check for the exit command.
            if (strcmp(cmd.c_str(), "exit") == 0 || strcmp(cmd.c_str(), "quit") == 0) {
                Logger().log(INFO) << "Shutting down the server";
                
                login_thread.interrupt();
                login_thread.join();

                zone_thread.interrupt();                
                zone_thread.join();

                break;
            }

            Logger().log(ERR) << "Command not found";
        }
    } catch (std::exception& e) {
        Logger().log(ERR) << e.what();
    }

    return 0;
}

