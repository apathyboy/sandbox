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

#ifndef OPENSWG_SOCKET_SERVER_H
#define OPENSWG_SOCKET_SERVER_H

#define MAX_RECV_SIZE 512

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <map>
#include <string>
#include <tr1/memory>

#include "NetworkAddress.h"

/** SWG Client Map
 *  This is a container for the clients that are currently connected to the server.
 */
class GalaxySession;
typedef std::map<std::string, std::tr1::shared_ptr<GalaxySession>> GalaxySessionMap;


/** Socket Server Exception structure
 *  This exception is thrown whenever there is an error that prevents 
 *	a method from executing. This allows any issues raised to be handled
 *	in the domain where they are relevant.
 */
struct SocketServerException {};


/** SocketServer class
 *  This class handles the sending and receiving of data from the client.
 */
class SocketServerImpl;
class SocketServer
{
public:
	SocketServer();
    explicit SocketServer(uint16_t port);
    virtual ~SocketServer();

	/** Initialize Server function
	 *	Stores the configuration file and prepares the socket for use.
	 */
	virtual void InitServer();

	/** Shutdown Server function
	 *	Handles any cleanup the server requires before exiting.
	 */
	void ShutdownServer();

	/** Run Server function
	 *	Begins listening on the port specified in the configuration until
	 *	the server status is no longer set to running.
	 */
	void Run();

	/** On Incoming Data function
	 *	Whenever information is received via the socket this function is
	 *	called to handle the data.
	 */
	void OnIncoming(NetworkAddress address, char *packet, size_t length);

	/** Updates the zone state.
	 */
	virtual void OnUpdate() = 0;

	/** Send Packet function
	 *	Sends a packet to the specified to the specified client.
	 */
    void SendPacket(const NetworkAddress& address, char *packet, unsigned short length);

	/** Add New SWG Client function
	 *	Adds a new swg client to the client map.
	 */
    std::tr1::shared_ptr<GalaxySession> AddGalaxySession(NetworkAddress address);

	const uint16_t port();

private:	
    std::tr1::shared_ptr<SocketServerImpl> pimpl_;

protected:	

	GalaxySessionMap mSessions;
	
	time_t mCurrentTime;
};

#endif // OPENSWG_SOCKET_SERVER_H