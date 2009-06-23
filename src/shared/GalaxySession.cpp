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
GalaxySession::GalaxySession(SocketServer* server, NetworkAddress address, std::string ip)
    : mShuttleState(0) 
    , mConnectionId(0)
    , mServerSequence(0)
    , mClientSequence(0)
    , mSequenceRecv(0)
    , mCrcSeed(0xDEADBABE)
    , mIp(ip)
    , mSocketAddress(address)
    , p_mSocketServer(server)
    , mPlayer(new Player())
{
    mPlayer->SetXPos(-1443);
    mPlayer->SetYPos(9);
    mPlayer->SetZPos(2771);
    mPlayer->SetLocation("naboo");
    mPlayer->SetStationId(653564567);
    mPlayer->SetMood(0);
    mPlayer->ToggleInitialized();
}
	
/** Handle Packet function
 *	Processes any packets that are sent to the server.
 */
void GalaxySession::HandlePacket(char *pData, size_t length)
{
	unsigned int opcode;
	// Try to handle the incoming packet.
	try
	{
		// Search for the opcode handler function and pass it the packet data.
		handlerFunc handler = OpcodeFactory::GetOpcodeHandler(pData, &opcode);
		handler(this, pData, length);
	}
	catch(...)
	{
		// Log any unknown opcodes.
		Logger().log(ERR) << "Unknown Opcode Found - Group: " << pData[1] << "  SWG: " << *(uint32_t*)(pData+2);
		SendOk();
	}
}


/** Send Packet function
 *	Sends a packet to the specified to the specified client.
 *
 *  @note: this is heavily borrowed from the swgemu team's similar function.
 */
void GalaxySession::SendPacket(char *pData, unsigned short length, bool encrypted, bool compressed, bool crc)
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

	p_mSocketServer->SendPacket(mSocketAddress, pData, length);

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
	unsigned short length;
	char *pData = loadPacket(pName, &length);
	
	// Add the server sequence to the packet and send the data.
	*(unsigned short*)(pData+2) = (unsigned short)htons(GetServerSequence());
	SendPacket(pData, length, true, compressed, true);

	IncrementServerSequence();	
}

void GalaxySession::SendHardPacket(char *packet, unsigned short length, bool compressed)
{	
	// Add the server sequence to the packet and send the data.
	*(unsigned short*)(packet+2) = (unsigned short)htons(GetServerSequence());
	SendPacket(packet, length, true, compressed, true);

	IncrementServerSequence();	
}

void GalaxySession::SendText(wchar_t *text, unsigned short length, uint64_t *moodId)
{
	// Load in the raw packet.
	unsigned short headerSize;
	char *header = loadPacket("packets\\Spatial\\PlayerChatHeader.txt", &headerSize);

	unsigned short footerSize;
	char *footer = loadPacket("packets\\Spatial\\PlayerChatFooter.txt", &footerSize);

	char *packet = new char[headerSize+footerSize+length];

	memcpy(packet, header, headerSize);

	*(unsigned short*)(packet+46) = length/2;
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
char* GalaxySession::PrepPacket(char* pData, unsigned short &nLength)
{
    bool comp = false;
    switch(pData[1]) //switch to do packet manip before passing to handlers
    {
        case 3:
        //"Multi-SOE Packet: "
        if(CrcTest(pData,nLength,mCrcSeed))
            Decrypt(pData,nLength,mCrcSeed);
        if (pData[2] == 'x')
        {
            comp = true;
            pData = Decompress(pData,nLength);
        }
        break;
        case 9:
        //"Data Channel: "
        Decrypt(pData,nLength,mCrcSeed);
        if (pData[2] == 'x')
        {
            comp = true;
            pData = Decompress(pData,nLength);
        }
        break;
        case 13:
        //"Fragmented: "
        Decrypt(pData,nLength,mCrcSeed);
        if (pData[2] == 'x')
        {
            comp = true;
            pData = Decompress(pData,nLength);
        }
        break;
        case 6:
        //"SOE Ping: "
        Decrypt(pData,nLength,mCrcSeed);
        break;
        case 7:
        //"Client Net-Status: "
        Decrypt(pData,nLength,mCrcSeed);
        if (pData[2] == 'x')
        {
            comp = true;
            pData = Decompress(pData,nLength);
        }
        break;
        case 21:
        //"Acknowledge: "
        Decrypt(pData,nLength,mCrcSeed);
        break;
        case 5:
        //"Disconnect: "
        Decrypt(pData,nLength,mCrcSeed);
        break;

        case 17:
        //"Future Packet: "
        Decrypt(pData,nLength,mCrcSeed);
        break;
        default:
        //"Unrecorded OP found: "
        break;
    }

	return pData;
}


std::tr1::shared_ptr<Player> GalaxySession::GetPlayer()
{
	/* If the player is not initialized, store the player settings.
	if (! mPlayer)
	{
		mPlayer = new Player();
		mPlayer->SetXPos(-1443);
		mPlayer->SetYPos(9);
		mPlayer->SetZPos(2771);
		mPlayer->SetLocation("naboo");
		mPlayer->ToggleInitialized();
		mPlayer->SetStationId(653564567);
		mPlayer->SetMood(4);
	}
	*/
	return mPlayer;
}

void GalaxySession::SendAck()
{
	// Load in the raw packet data.
	unsigned short length;
	char *packet = loadPacket("packets\\SendAcknowledge.txt", &length);

	// Add the sequence to the packet.
	unsigned short *ptr = (unsigned short*)(packet+2);
	*ptr = GetClientSequence();

	// Send out the packet.
	SendPacket(packet, length, true);
}

void GalaxySession::SendOk()
{
	// Load in the raw packet data.
	unsigned short length;
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