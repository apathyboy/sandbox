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

#include "SocketServer.h"

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

class LoginServer : public SocketServer
{
public:
    LoginServer();
    explicit LoginServer(uint16_t port);
    virtual ~LoginServer();

	/**	Run the server
	 *	This begins the login server loop. The sole purpose of the login
	 *	server is to listen for and accept data as well as sending it
	 *	out to the client. The data is then passed off to the appropriate
	 *	opcode handler.
	 */
	virtual void Run();
	virtual void OnUpdate() { return; }
};