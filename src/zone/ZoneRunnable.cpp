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

#include "ZoneRunnable.h"
#include "ZoneServer.h"
#include "Logger.h"

extern bool running;

void ZoneRunnable::run(uint16_t port) {
	ZoneServer zoned(port);
	try {
		zoned.InitServer();
		zoned.Run();
	} catch(SocketServerException()) {
		running = false;
		Logger().log(ERR) << "ERROR: Unable to run the Zone Server!";
	}

	Logger().log(ERR) << "Exiting the zoned thread";
}