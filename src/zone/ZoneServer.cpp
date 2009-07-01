/*
 * OpenSWG Combat Upgrade Sandbox - ZoneServer.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "ZoneServer.h"
#include "Logger.h"
#include "Session.h"

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

    SessionMap::iterator end = sessions_.end();
    for (SessionMap::iterator i = sessions_.begin(); i !=end; ++i) {
        std::tr1::shared_ptr<Session> session = (*i).second;
        session->sendHardcodedPacket(packet_name, false);
	}
}

