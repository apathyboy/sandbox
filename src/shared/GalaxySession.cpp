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
    , mShuttleState(0) 
    , mConnectionId(0)
    , mServerSequence(0)
    , mClientSequence(0)
    , mSequenceRecv(0)
    , mCrcSeed(0xDEADBABE)
{
    player_->SetXPos(-1443);
    player_->SetYPos(9);
    player_->SetZPos(2771);
    player_->SetLocation("naboo");
    player_->SetStationId(653564567);
    player_->SetMood(0);
    player_->ToggleInitialized();
}


const SocketServer * const GalaxySession::server() const
{
    return socket_server_;
}


std::tr1::shared_ptr<Player> GalaxySession::player()
{
	return player_;
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
	/* Uncomment to display all outgoing packet data.
	printf("OUTGOING PACKET\nPacket Size: %d \nPacket Data: \n", length);
	for(int k = 0; k < length; k++)
	{
		printf("0x%02x ",(unsigned char)*(pData+k));
	}
	printf("\n\n");
	*/

	// @note: this is taken almost straight from the swgemu code, look into better integration
	// of the encryption and compression routines.
    if(compressed)
    {
        pData = Compress(pData, length);
    }

    if(encrypted)
    {
        Encrypt(pData, length, mCrcSeed);
    }
    if(crc)
    {
		AppendCRC(pData, length, mCrcSeed);
    }

    std::tr1::shared_ptr<ByteBuffer> message(new ByteBuffer(reinterpret_cast<unsigned char*>(pData), length));
	socket_server_->sendPacket(socket_address_, message);

    if(compressed)
    {
        delete [] pData;
    }
}

/**	Send Hard Packet
 *	Sends a hardcoded packet to the specified client.
 */
void GalaxySession::SendHardPacket(char *pName, bool compressed)
{
	// Load in the raw packet.
	uint16_t length;
	char *pData = loadPacket(pName, &length);
	
	// Add the server sequence to the packet and send the data.
	*(uint16_t*)(pData+2) = (uint16_t)htons(GetServerSequence());
	SendPacket(pData, length, true, compressed, true);

	IncrementServerSequence();	
}

void GalaxySession::SendHardPacket(char *packet, unsigned short length, bool compressed)
{	
	// Add the server sequence to the packet and send the data.
	*(uint16_t*)(packet+2) = (uint16_t)htons(GetServerSequence());
	SendPacket(packet, length, true, compressed, true);

	IncrementServerSequence();	
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
            if(CrcTest(&pData[0],nLength,mCrcSeed))
                Decrypt(&pData[0],nLength,mCrcSeed);
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
            Decrypt(&pData[0],nLength,mCrcSeed);
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
            Decrypt(&pData[0],nLength,mCrcSeed);
            break;

        default:
            //"Unrecorded OP found: "
            break;
    }

    ByteBuffer tmp(reinterpret_cast<uint8_t*>(&pData[0]), pData.size());
    packet->swap(tmp);
}

void GalaxySession::SendAck()
{
	// Load in the raw packet data.
	uint16_t length;
	char *packet = loadPacket("packets\\SendAcknowledge.txt", &length);

	// Add the sequence to the packet.
	uint16_t *ptr = (uint16_t*)(packet+2);
	*ptr = GetClientSequence();

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
	if (mShuttleState == SHUTTLE_LANDED)
	{
		SendHardPacket("packets\\Actions\\KerenStarshipTakeoff.txt", false);
		mShuttleState = SHUTTLE_DEPARTED;
	}
	else
	{
		SendHardPacket("packets\\Actions\\KerenStarshipLand.txt", false);
		mShuttleState = SHUTTLE_LANDED;
	}
}