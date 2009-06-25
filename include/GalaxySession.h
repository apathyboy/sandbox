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

	/** Handle Packet function
	 *	Processes any packets that are sent to the server.
	 */
	void HandlePacket(std::tr1::shared_ptr<ByteBuffer> packet);

	/** Send Packet function
	 *	Sends a packet to the specified to the specified client.
	 *
	 *  @note: this is heavily borrowed from the swgemu team's similar function.
	 */
    void SendPacket(char *pData, uint16_t length, bool encrypted = false, bool compressed = false, bool crc = true);

	/**	Send Hard Packet
	 *	Sends a hardcoded packet to the specified client.
	 */
	void SendHardPacket(char *pName, bool compressed);
	void SendHardPacket(char *packet, uint16_t length, bool compressed);

	/**	Send Text
	 *	Sends text to the client and the clients in the nearby area.
	 */
	void SendText(wchar_t *text, uint16_t length, uint64_t *moodId);

	/** Send Acknowledgement
	 *	Sends an acknowledgement to the client that a message was received.
	 */
	void SendAck();

	/** Send Ok
	 *	Sends an OK statement to the client.
	 */
	void SendOk();

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	uint32_t GetCrcSeed() { return mCrcSeed; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	uint32_t GetConnectionId() { return mConnectionId; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	void SetConnectionId(uint32_t connectionId) { mConnectionId = connectionId; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	uint16_t GetServerSequence() { return mServerSequence; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	void SetServerSequence(uint16_t serverSequence) { mServerSequence = serverSequence; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	void IncrementServerSequence() { mServerSequence++; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	uint16_t GetClientSequence() { return mClientSequence; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	void SetClientSequence(uint16_t clientSequence) { mClientSequence = clientSequence; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	uint16_t GetSequenceRecv() { return mSequenceRecv; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	void SetSequenceRecv(uint16_t sequenceRecv) { mSequenceRecv = sequenceRecv; }

	/** Prepare Packet
	 *	Preapres an incoming packet for use. This decompresses
	 *	and decrypts the packets and returns the formatted data.
	 */
    void PrepPacket(std::tr1::shared_ptr<ByteBuffer> packet);

	/** Get Player
	 *	Returns an instance of the player associated with this session.
	 */
    std::tr1::shared_ptr<Player> GetPlayer();

	void Update(time_t currentTime);
	void SendShuttleUpdate();

private:
    /* Disable unused compiler generated methods */
    GalaxySession();
    GalaxySession(const GalaxySession&);
    GalaxySession& operator=(const GalaxySession&);    

	NetworkAddress socket_address_;
	const SocketServer* socket_server_;
    std::tr1::shared_ptr<Player> mPlayer;
	uint8_t mShuttleState;
	uint16_t mServerSequence;
	uint16_t mClientSequence;
	uint16_t mSequenceRecv;
	uint32_t mConnectionId;
    uint32_t mCrcSeed;
};

#endif // OPENSWG_GALAXY_SESSION_H