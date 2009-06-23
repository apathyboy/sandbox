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


#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include "SocketServer.h"

class ZoneServer : public SocketServer
{
public:
public:
    ZoneServer();
    explicit ZoneServer(uint16_t port);
    virtual ~ZoneServer();

	/** Initialize Server function
	 *	Stores the configuration file and prepares the socket for use.
	 */
	virtual void InitServer();

	/**	Run the server
	 *	This begins the zone server loop. The sole purpose of the zone
	 *	server is to listen for and accept data as well as sending it
	 *	out to the client. The data is then passed off to the appropriate
	 *	opcode handler.
	 */
	virtual void Run();
	virtual void OnUpdate();
};