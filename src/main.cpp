/**
 * *********************************************************************
 * OpenSWG Sandbox Server
 * Copyright (C) 2006 OpenSWG Team <http://www.openswg.com>
 * *********************************************************************
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * To read the license please visit http://www.gnu.org/copyleft/gpl.html
 * *********************************************************************
 */

#include <exception>
#include <iostream>
#include <string>

#include <boost/thread.hpp>

#include "Logger.h"
#include "LoginServer.h"
#include "ZoneServer.h"
#include "ProgramOptions.h"

namespace po = boost::program_options;

/* Program Constants */
const char* config_filename = "openswg.cfg";

// Run the OpenSWG Sandbox Server.
int main(int argc, char *argv[])
{      
    // Load program options and print program info (if requested).
    po::variables_map sandbox_options = buildProgramOptions(argc, argv, config_filename);
   
	// Send server header output.
	//sLog.logTitle("OpenSWG Combat Upgrade Sandbox v1.0");
	//sLog.logTitle("Copyright (c) 2006 OpenSWG Development Team\n");
	//sLog.logString("Server starting with configuration file: %s.\n", config_filename);
    std::cout << "OpenSWG Combat Upgrade Sandbox v2.0" << std::endl;
    std::cout << "Copyright (c) 2009 Spira Studios LLC" << std::endl << std::endl;
    
    Logger().log(INFO) << "Server starting with configuration file: " << config_filename;

	// Begin the main execution phase. This begins by starting the server
	// threads and then running until the exit command is received.
	try {		
        LoginServer login_server(sandbox_options["login_port"].as<uint16_t>());
        boost::thread login_thread(std::tr1::bind(&LoginServer::Run, &login_server));

        ZoneServer zone_server(sandbox_options["zone_port"].as<uint16_t>());
        boost::thread zone_thread(std::tr1::bind(&ZoneServer::Run, &zone_server));

		for(;;)
		{
            std::string cmd;
			std::cin >> cmd;

			// Check for the exit command.
			if (strcmp(cmd.c_str(), "exit") == 0 || strcmp(cmd.c_str(), "quit") == 0)
			{
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