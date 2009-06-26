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

#include "GalaxySession.h"
#include "OpcodeFactory.h"
#include "Logger.h"
#include "PacketTools.h"

/**	Galaxy Session constructor
 *	Takes the data necessary for the GalaxySession class to function.
 */
GalaxySession::GalaxySession(const SocketServer * const server, const NetworkAddress& address)
    : socket_address_(address)
    , socket_server_(server)
    , player_(new Player())
    , shuttle_state_(0)
    , server_sequence_(0)
    , client_sequence_(0)
    , received_sequence_(0)
    , connection_id_(0)
    , crc_seed_(0xDEADBABE)
{
    // Initialize the player to a default location and state. 
    // @todo: This information should be pulled from storage
    player_->position(Vector3<int>(-1443, 9, 2771));
    player_->stationId(653564567);
    player_->locationName("naboo");
    player_->mood(0);
}


const SocketServer * const GalaxySession::server() const
{
    return socket_server_;
}


std::tr1::shared_ptr<Player> GalaxySession::player()
{
	return player_;
}


uint16_t GalaxySession::serverSequence() const
{
    return server_sequence_;
}


uint16_t GalaxySession::serverSequence(uint16_t sequence)
{
    server_sequence_ = sequence;
    return server_sequence_;
}


uint16_t GalaxySession::clientSequence() const
{
    return client_sequence_;
}


uint16_t GalaxySession::clientSequence(uint16_t sequence)
{
    client_sequence_ = sequence;
    return client_sequence_;
}


uint16_t GalaxySession::receivedSequence() const
{
    return received_sequence_;
}


uint16_t GalaxySession::receivedSequence(uint16_t sequence)
{
    received_sequence_ = sequence;
    return received_sequence_;
}


uint32_t GalaxySession::connectionId() const
{
    return connection_id_;
}


uint32_t GalaxySession::connectionId(uint32_t id)
{
    connection_id_ = id;
    return connection_id_;
}


uint32_t GalaxySession::crcSeed() const
{
    return crc_seed_;
}


uint32_t GalaxySession::crcSeed(uint32_t seed)
{
    crc_seed_ = seed;
    return crc_seed_;
}	


void GalaxySession::prepPacket(std::tr1::shared_ptr<ByteBuffer> packet)
{    
    if(CrcTest(packet, crc_seed_)) {
        Decrypt(packet, crc_seed_);
    }

    if (packet->peekAt<uint8_t>(2) == 'x') {
        Decompress(packet);
    }

    //Logger().log(INFO) << "Incoming Packet: " << std::endl << tmp << std::endl;    
}


void GalaxySession::sendPacket(std::tr1::shared_ptr<ByteBuffer> packet, bool encrypt, bool compress, bool crc)
{
    //Logger().log(INFO) << "Outgoing Packet: " << std::endl << *packet << std::endl;

    if (compress) Compress(packet);
    if (encrypt)  Encrypt(packet, crc_seed_);    
    if (crc)	  AppendCrc(packet, crc_seed_);

    socket_server_->sendPacket(socket_address_, packet);
}


/** Handle Packet function
 *	Processes any packets that are sent to the server.
 */
void GalaxySession::HandlePacket(std::tr1::shared_ptr<ByteBuffer> packet)
{
	unsigned int opcode;
	// Try to handle the incoming packet.
	try
	{
		// Search for the opcode handler function and pass it the packet data.
        handlerFunc handler = OpcodeFactory::GetOpcodeHandler(packet->data(), &opcode);
		handler(this, packet->data(), packet->size());
	}
	catch(...)
	{
		// Log any unknown opcodes.
        Logger().log(ERR) << "Unidentified packet:" << std::endl << *packet;
		SendOk();
	}
}


/** Send Packet function
 *	Sends a packet to the specified to the specified client.
 *
 *  @note: this is heavily borrowed from the swgemu team's similar function.
 */
void GalaxySession::SendPacket(char *pData, uint16_t length, bool encrypt, bool compress, bool crc)
{
    std::tr1::shared_ptr<ByteBuffer> message(new ByteBuffer(reinterpret_cast<unsigned char*>(pData), length));
	//Logger().log(INFO) << "Outgoing Packet: " << std::endl << *message << std::endl;

    if (compress) Compress(message);
    if (encrypt)  Encrypt(message, crc_seed_);    
    if (crc)	  AppendCrc(message, crc_seed_);

    socket_server_->sendPacket(socket_address_, message);
}


/**	Send Hard Packet
 *	Sends a hardcoded packet to the specified client.
 */
void GalaxySession::SendHardPacket(const std::string& name, bool compressed)
{        
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile(name);

    packet->writeAt<uint16_t>(2, static_cast<uint16_t>(htons(server_sequence_)));
    sendPacket(packet, true, compressed, true);

    ++server_sequence_;
}

void GalaxySession::SendHardPacket(char *packet, unsigned short length, bool compressed)
{	
	// Add the server sequence to the packet and send the data.
	*(uint16_t*)(packet+2) = (uint16_t)htons(server_sequence_);
	SendPacket(packet, length, true, compressed, true);

	++server_sequence_;	
}

void GalaxySession::SendText(wchar_t *text, unsigned short length, uint64_t *moodId)
{
	// Load in the raw packet.
	uint16_t headerSize;
	char *header = loadPacket("packets\\Spatial\\PlayerChatHeader.txt", &headerSize);

	uint16_t footerSize;
	char *footer = loadPacket("packets\\Spatial\\PlayerChatFooter.txt", &footerSize);

	char *packet = new char[headerSize+footerSize+length];

	memcpy(packet, header, headerSize);

	*(uint16_t*)(packet+46) = length/2;
	memcpy(packet+50, text, length);
	memcpy(packet+50+length, footer, footerSize);

	*(uint16_t *)(packet+50+length+2) = moodId[1];
	*(uint16_t *)(packet+50+length+4) = moodId[2];

	SendHardPacket(packet, (headerSize + footerSize + length), true);

	delete [] packet;		
}


void GalaxySession::SendAck()
{
	// Load in the raw packet data.
	uint16_t length;
	char *packet = loadPacket("packets\\SendAcknowledge.txt", &length);

	// Add the sequence to the packet.
	uint16_t *ptr = (uint16_t*)(packet+2);
	*ptr = client_sequence_;

	// Send out the packet.
	SendPacket(packet, length, true);
}

void GalaxySession::SendOk()
{
	// Load in the raw packet data.
	uint16_t length;
	char *packet = loadPacket("packets\\OkPacket.txt", &length);

	// Send out the packet.
	SendPacket(packet, length, true);
}



void GalaxySession::Update(time_t currentTime)
{
	static time_t lastShuttleTime = currentTime;

	if ((currentTime - lastShuttleTime) >= (1*60)) // Replace the 1 with a call to configuration.
	{
		lastShuttleTime = currentTime;
		SendShuttleUpdate();
	}
}


void GalaxySession::SendShuttleUpdate()
{
	if (shuttle_state_ == SHUTTLE_LANDED)
	{
		SendHardPacket("packets\\Actions\\KerenStarshipTakeoff.txt", false);
		shuttle_state_ = SHUTTLE_DEPARTED;
	}
	else
	{
		SendHardPacket("packets\\Actions\\KerenStarshipLand.txt", false);
		shuttle_state_ = SHUTTLE_LANDED;
	}
}