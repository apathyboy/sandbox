/*
 * OpenSWG Combat Upgrade Sandbox - ZoneServer.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "ZoneServer.h"
#include "Logger.h"
#include "Session.h"
#include "SOEHandler.h"
#include "LoginHandler.h"
#include "SpatialHandler.h"
#include "ZoneInsertionHandler.h"

enum ShuttleStates
{
	SHUTTLE_LANDED		= 0,
	SHUTTLE_DEPARTED	= 2
};


ZoneServer::ZoneServer(uint16_t port)
    : SocketServer(port)
    , protocol_()
    , shuttle_state_(0)
{}


ZoneServer::~ZoneServer()
{}


uint8_t ZoneServer::shuttleState() const
{
    return shuttle_state_;
}


void ZoneServer::shuttleState(uint8_t state)
{
    shuttle_state_ = state;
}


void ZoneServer::sendShuttleUpdate()
{
    std::string packet_name;

	if (shuttle_state_ == SHUTTLE_LANDED) {
        packet_name = "packets\\Actions\\KerenStarshipTakeoff.txt";
		shuttleState(SHUTTLE_DEPARTED);
	} else {
        packet_name = "packets\\Actions\\KerenStarshipLand.txt";
		shuttleState(SHUTTLE_LANDED);
	}

    SessionMap::iterator end = sessions_.end();
    for (SessionMap::iterator i = sessions_.begin(); i !=end; ++i) {
        std::tr1::shared_ptr<Session> session = (*i).second;
        session->sendHardcodedPacket(packet_name, false);
	}
}


std::tr1::shared_ptr<Session> ZoneServer::addSession(const NetworkAddress& address)
{
    Logger().log(INFO) << "Adding session for [" << address << "]";

	// Create a new session and store it in the session map. 
    std::tr1::shared_ptr<Session> session(new Session(this, address, protocol_));
	sessions_[address] = session;

	// Return the new session.
	return session;
}


void ZoneServer::onIncoming(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> message)
{
	// Attempt to find the client in the session map.
	SessionMap::iterator i = sessions_.find(address);

	// If a session exists retrieve it from the session map otherwise verify
    // the message is a session request and create a new one.
    std::tr1::shared_ptr<Session> session;
	if (i != sessions_.end()) {
        session = (*i).second;
	} else if (message->peek<uint16_t>(true) == 0x0001) {        
        session = addSession(address);
	} else {
        Logger().log(ERR) << "Unexpected message from [" << address << ": " << std::endl << message;	
        return;
	}

	session->handlePacket(message);
}


void ZoneServer::onUpdate()
{
    time_t current_time = currentTime();    

	static time_t last_cleanup_time = current_time;
	static time_t last_shuttle_time = current_time;

    // Check to see if enough time has passed then update the session map.
	if ((current_time - last_cleanup_time) >= 1) {

        SessionMap::iterator end = sessions_.end();
        for (SessionMap::iterator i = sessions_.begin(); i !=end; ++i) {
            std::tr1::shared_ptr<Session> session = (*i).second;
			session->update(current_time);
		}

	    last_cleanup_time = current_time;
	}

	if ((current_time - last_shuttle_time) >= (1*60)) // Replace the 1 with a call to configuration.
	{
		last_shuttle_time = current_time;
		sendShuttleUpdate();
	}
}


void ZoneServer::initializeProtocol()
{
    protocol_.addHandler(1, std::tr1::bind(&HandleSessionRequest, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(3, std::tr1::bind(&HandleMultiPacket, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(5, std::tr1::bind(&HandleDisconnect, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(6, std::tr1::bind(&HandlePing, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(7, std::tr1::bind(&HandleNetStatus, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(9, std::tr1::bind(&HandleDataChannel, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(21, std::tr1::bind(&HandleAcknowledge, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    protocol_.addHandler(0x41131F96, std::tr1::bind(&HandleAuthentication, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    protocol_.addHandler(0xD5899226, std::tr1::bind(&HandleSession, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    protocol_.addHandler(0xB5098D76, std::tr1::bind(&HandleZoneInsertionRequest, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0x4C3D2CFA, std::tr1::bind(&HandleLoadTerrain, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0x43FD1C22, std::tr1::bind(&HandleLoadDone, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0xCA88FBAD, std::tr1::bind(&HandleClientReady, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0x48f493c5, std::tr1::bind(&HandleClientReady, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

		// Spatial
    protocol_.addHandler(0x80ce5e46, std::tr1::bind(&HandleSpatial, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0xb719fa26, std::tr1::bind(&HandleSit, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0xa8a25c79, std::tr1::bind(&HandleStand, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0x7c8d63d4, std::tr1::bind(&HandleSpatialChat, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0xbd8d02af, std::tr1::bind(&HandleProne, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0x01b48b26, std::tr1::bind(&HandleKneel, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0x7759F35E, std::tr1::bind(&HandleMood, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(0x32CF1BEE, std::tr1::bind(&HandleEmote, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}

