/*
 * OpenSWG Combat Upgrade Sandbox - LoginServer.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "LoginServer.h"
#include "Logger.h"
#include "SOEHandler.h"
#include "LoginHandler.h"
#include "Session.h"

LoginServer::LoginServer(uint16_t port)
    : SocketServer(port)
    , protocol_()
{}


LoginServer::~LoginServer()
{}


std::tr1::shared_ptr<Session> LoginServer::addSession(const NetworkAddress& address)
{
    Logger().log(INFO) << "Adding session for [" << address << "]";

	// Create a new session and store it in the session map. 
    std::tr1::shared_ptr<Session> session(new Session(this, address, protocol_));
	sessions_[address] = session;

	// Return the new session.
	return session;
}


void LoginServer::onIncoming(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> message)
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


void LoginServer::onUpdate()
{
    time_t current_time = currentTime(); 
	static time_t last_cleanup_time = current_time;

    // Check to see if enough time has passed then update the session map.
	if ((current_time - last_cleanup_time) >= 1) {

        SessionMap::iterator end = sessions_.end();
        for (SessionMap::iterator i = sessions_.begin(); i !=end; ++i) {
            std::tr1::shared_ptr<Session> session = (*i).second;
			session->update(current_time);
		}

	    last_cleanup_time = current_time;
	}
}


void LoginServer::initializeProtocol()
{
    protocol_.addHandler(1, std::tr1::bind(&HandleSessionRequest, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(3, std::tr1::bind(&HandleMultiPacket, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(5,  std::tr1::bind(&HandleDisconnect, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(6,  std::tr1::bind(&HandlePing, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(7, std::tr1::bind(&HandleNetStatus, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(9,  std::tr1::bind(&HandleDataChannel, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(21, std::tr1::bind(&HandleAcknowledge, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    protocol_.addHandler(0x41131F96, std::tr1::bind(&HandleAuthentication, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    protocol_.addHandler(0xD5899226, std::tr1::bind(&HandleSession, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}

