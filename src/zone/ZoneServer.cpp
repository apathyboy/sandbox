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

#include "ZoneServer.h"
#include "Logger.h"
#include "GalaxySession.h"

enum ShuttleStates
{
	SHUTTLE_LANDED		= 0,
	SHUTTLE_DEPARTED	= 2
};

ZoneServer::ZoneServer(uint16_t port)
    : SocketServer(port)
    , shuttle_state_(0)
{}

ZoneServer::~ZoneServer()
{}

void ZoneServer::onUpdate()
{
    time_t current_time = currentTime();
	static time_t last_shuttle_time = current_time;

	if ((current_time - last_shuttle_time) >= (1*60)) // Replace the 1 with a call to configuration.
	{
		last_shuttle_time = current_time;
		sendShuttleUpdate();
	}
}

void ZoneServer::sendShuttleUpdate()
{
    std::string packet_name;

	if (shuttle_state_ == SHUTTLE_LANDED) {
        packet_name = "packets\\Actions\\KerenStarshipTakeoff.txt";
		shuttle_state_ = SHUTTLE_DEPARTED;
	} else {
        packet_name = "packets\\Actions\\KerenStarshipLand.txt";
		shuttle_state_ = SHUTTLE_LANDED;
	}

    GalaxySessionMap::iterator end = sessions_.end();
    for (GalaxySessionMap::iterator i = sessions_.begin(); i !=end; ++i) {
        std::tr1::shared_ptr<GalaxySession> session = (*i).second;
        session->sendHardcodedPacket(packet_name, false);
	}
}

