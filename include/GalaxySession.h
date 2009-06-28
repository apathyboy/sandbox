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

#ifndef OPENSWG_GALAXY_SESSION_H
#define OPENSWG_GALAXY_SESSION_H

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <tr1/memory>

#include "ByteBuffer.h"
#include "Player.h"
#include "SocketServer.h"

enum ShuttleStates
{
	SHUTTLE_LANDED		= 0,
	SHUTTLE_DEPARTED	= 2
};

/** Galaxy Session class
 *	Manages the client -> server and server <- client session.
 */
class GalaxySession
{
public:
	/**	Galaxy Session constructor
	 *	Takes the data necessary for the GalaxySession class to function.
	 */
	GalaxySession(const SocketServer * const server, const NetworkAddress& address);

    const SocketServer * const server() const;
    std::tr1::shared_ptr<Player> player();

    uint16_t serverSequence() const;
    uint16_t serverSequence(uint16_t sequence);

    uint16_t clientSequence() const;
    uint16_t clientSequence(uint16_t sequence);

    uint16_t receivedSequence() const;
    uint16_t receivedSequence(uint16_t sequence);

    uint32_t connectionId() const;
    uint32_t connectionId(uint32_t id);

    uint32_t crcSeed() const;
    uint32_t crcSeed(uint32_t seed);
    
    void sendHardcodedPacket(const std::string& name, bool compressed);
    void sendHardcodedPacket(std::tr1::shared_ptr<ByteBuffer> packet, bool compressed);

    void sendToRemote(std::tr1::shared_ptr<ByteBuffer> packet, bool encrypt = false, bool compress = false, bool crc = true) const;

    void sendHeartbeat() const;
    void sendAcknowledge() const;

    void sendText(const std::wstring& text, uint64_t* moodId);


    /** Handle Packet function
	 *	Processes any packets that are sent to the server.
	 */
	void HandlePacket(std::tr1::shared_ptr<ByteBuffer> packet);

	/** Send Packet function
	 *	Sends a packet to the specified to the specified client.
	 *
	 *  @note: this is heavily borrowed from the swgemu team's similar function.
	 */
    void SendPacket(char *pData, uint16_t length, bool encrypt = false, bool compress = false, bool crc = true);

	/**	Send Hard Packet
	 *	Sends a hardcoded packet to the specified client.
	 */
	void SendHardPacket(char *packet, uint16_t length, bool compressed);

	void Update(time_t currentTime);
	void SendShuttleUpdate();

private:
    /* Disable unused compiler generated methods */
    GalaxySession();
    GalaxySession(const GalaxySession&);
    GalaxySession& operator=(const GalaxySession&);    

	NetworkAddress socket_address_;
	const SocketServer* socket_server_;
    std::tr1::shared_ptr<Player> player_;

	uint8_t  shuttle_state_;
	uint16_t server_sequence_;
	uint16_t client_sequence_;
	uint16_t received_sequence_;
	uint32_t connection_id_;
    uint32_t crc_seed_;
};

#endif // OPENSWG_GALAXY_SESSION_H