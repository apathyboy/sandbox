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
        Logger().log(ERR) << "Unknown Opcode Found - Group: " << packet->read<uint16_t>() << "  SWG: " << packet->read<uint32_t>();
		SendOk();
	}
}


/** Send Packet function
 *	Sends a packet to the specified to the specified client.
 *
 *  @note: this is heavily borrowed from the swgemu team's similar function.
 */
void GalaxySession::SendPacket(char *pData, uint16_t length, bool encrypted, bool compressed, bool crc)
{
    ByteBuffer tmp(reinterpret_cast<unsigned char*>(pData), length);    
    Logger().log(INFO) << "Outgoing Packet: " << std::endl << tmp << std::endl;

	// @note: this is taken almost straight from the swgemu code, look into better integration
	// of the encryption and compression routines.
    if(compressed)
    {
        pData = Compress(pData, length);
    }
    /*
    if(encrypted)
    {
        Encrypt(pData, length, crc_seed_);
    }
    */
    std::tr1::shared_ptr<ByteBuffer> message(new ByteBuffer(reinterpret_cast<unsigned char*>(pData), length));
	  
    if(encrypted)
    {
        Encrypt(message, crc_seed_);
    }
    
    if(crc)
    {
		AppendCrc(message, crc_seed_);
    }
    
    socket_server_->sendPacket(socket_address_, message);

}

/**	Send Hard Packet
 *	Sends a hardcoded packet to the specified client.
 */
void GalaxySession::SendHardPacket(const std::string& name, bool compressed)
{
	// Load in the raw packet.
	uint16_t length;
	char *pData = loadPacket(name, &length);
	
	// Add the server sequence to the packet and send the data.
	*(uint16_t*)(pData+2) = (uint16_t)htons(server_sequence_);
	SendPacket(pData, length, true, compressed, true);

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


/** The body of this function was written by SWGEmu. It may be replaced
 *	in the future to better integrate with the rest of the OpenSWG
 *	code.
 *	Copyright (C) 2006 Team SWGEmu <http://www.swgemu.com>
 */
void GalaxySession::PrepPacket(std::tr1::shared_ptr<ByteBuffer> packet)
{
    std::vector<char> pData(packet->data(), packet->data() + packet->size());
    uint16_t nLength = packet->size();

    switch(pData[1]) //switch to do packet manip before passing to handlers
    {
        case 3:
            //"Multi-SOE Packet: "
            if(CrcTest(&pData[0],nLength,crc_seed_))
                Decrypt(&pData[0],nLength,crc_seed_);
            if (pData[2] == 'x')
            {
                std::vector<char> tmp(Decompress(pData));
                std::swap(pData, tmp);
            }
            break;
        
        case 7:
        case 9:
        case 13:
            //"Fragmented: "
            Decrypt(&pData[0],nLength,crc_seed_);
            if (pData[2] == 'x')
            {
                std::vector<char> tmp(Decompress(pData));
                std::swap(pData, tmp);
            }
            break;

        case 6:
        case 21:
        case 5:
        case 17:
            //"Future Packet: "
            Decrypt(&pData[0],nLength,crc_seed_);
            break;

        default:
            //"Unrecorded OP found: "
            break;
    }

    ByteBuffer tmp(reinterpret_cast<uint8_t*>(&pData[0]), pData.size());
    packet->swap(tmp);

    Logger().log(INFO) << "Incoming Packet: " << std::endl << tmp << std::endl;    
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