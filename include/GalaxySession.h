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
	GalaxySession(SocketServer *server, NetworkAddress address, std::string ip);

	/** Handle Packet function
	 *	Processes any packets that are sent to the server.
	 */
	void HandlePacket(std::tr1::shared_ptr<ByteBuffer> packet);

	/** Send Packet function
	 *	Sends a packet to the specified to the specified client.
	 *
	 *  @note: this is heavily borrowed from the swgemu team's similar function.
	 */
    void SendPacket(char *pData, unsigned short length, bool encrypted = false, bool compressed = false, bool crc = true);

	/**	Send Hard Packet
	 *	Sends a hardcoded packet to the specified client.
	 */
	void SendHardPacket(char *pName, bool compressed);
	void SendHardPacket(char *packet, unsigned short length, bool compressed);

	/**	Send Text
	 *	Sends text to the client and the clients in the nearby area.
	 */
	void SendText(wchar_t *text, unsigned short length, uint64_t *moodId);

	/** Send Acknowledgement
	 *	Sends an acknowledgement to the client that a message was received.
	 */
	void SendAck();

	/** Send Ok
	 *	Sends an OK statement to the client.
	 */
	void SendOk();

	/** Get Ip function
	 *	Returns the ip address for the current client.
	 */
	std::string GetIp() { return mIp; }

	/** Get Socket Server function
	 *	Returns the current instance of the socket server managing this client.
	 */
	SocketServer* GetSocketServer() { return p_mSocketServer; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	uint32_t GetCrcSeed() { return mCrcSeed; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	const unsigned int GetConnectionId() { return mConnectionId; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	void SetConnectionId(unsigned int connectionId) { mConnectionId = connectionId; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	const unsigned short GetServerSequence() { return mServerSequence; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	void SetServerSequence(unsigned short serverSequence) { mServerSequence = serverSequence; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	void IncrementServerSequence() { mServerSequence++; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	const unsigned short GetClientSequence() { return mClientSequence; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	void SetClientSequence(unsigned short clientSequence) { mClientSequence = clientSequence; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	const unsigned short GetSequenceRecv() { return mSequenceRecv; }

	/** Get Crc Seed function
	 *	Returns the encryption key.
	 */
	void SetSequenceRecv(unsigned short sequenceRecv) { mSequenceRecv = sequenceRecv; }

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
	uint8_t mShuttleState;
	unsigned int mConnectionId;
	unsigned short mServerSequence;
	unsigned short mClientSequence;
	unsigned short mSequenceRecv;
    uint32_t mCrcSeed;
	std::string mIp;
	NetworkAddress mSocketAddress;
	SocketServer* p_mSocketServer;
    std::tr1::shared_ptr<Player> mPlayer;
};

#endif // OPENSWG_GALAXY_SESSION_H