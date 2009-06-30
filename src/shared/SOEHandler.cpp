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

#include "SOEHandler.h"
#include "GalaxySession.h"
#include "PacketTools.h"
#include "Logger.h"

void HandleSessionRequest(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message)
{ 	
    session.connectionId(message->peekAt<uint32_t>(6));

    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\SOE\\SessionResponse.txt");

    packet->writeAt<uint32_t>(2, session.connectionId());
    packet->writeAt<uint32_t>(6, htonl(session.crcSeed()));

    session.sendToRemote(packet, false, false, false);

	// Send the connection packet.
	if (session.server()->port() == 44453) 
		session.sendHardcodedPacket("packets\\SOE\\LoginServer.txt", false);		
	else
		session.sendHardcodedPacket("packets\\SOE\\ConnectionServer.txt", false);
}


void HandleNetStatus(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message)
{     
    uint32_t tick = message->peekAt<uint32_t>(2);
    
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\SOE\\NetStatus.txt");

    packet->writeAt<uint16_t>(2, tick);

    session.sendToRemote(packet, true, true, false);
}


void HandleMultiPacket(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message)
{       
    message->read<uint16_t>();

    // Loop through the message until the compression bit is reached.
    while (message->readPosition() < message->size() - 4)
    {
        uint8_t segment_size = message->read<uint8_t>();
        
        // If the segment size is 255+ check the next bit, a 0x01 indicates
        // over 255 and that the next bit should be added to the total
        // size. If 0x00 then the next bit should be skipped.
        if (segment_size == 0xFF) {
            if (message->read<uint8_t>() == 0x01) {
                segment_size += message->read<uint8_t>();
            } else {
                message->read<uint8_t>();
            }
        }
    
        std::tr1::shared_ptr<ByteBuffer> segment(new ByteBuffer(message->data()+message->readPosition(), segment_size));
        message->readPosition(message->readPosition() + segment_size);

		session.handlePacket(segment);
    }
}


void HandleAcknowledge(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message)
{	
    message->read<uint16_t>();

    session.receivedSequence(message->read<uint16_t>());
    session.clientSequence(message->read<uint16_t>());
}


void HandleDataChannel(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message)
{	
    Logger().log(INFO) << "Datachannel 0:" << std::endl << *message;

    message->read<uint16_t>();

    session.clientSequence(message->read<uint16_t>());
    session.sendAcknowledge();

    if (ntohs(message->peek<uint16_t>()) == 0x0019) {
        message->read<uint16_t>();

        // Loop through the message until the compression bit is reached.
        while (message->readPosition() < message->size() - 3)
        {
            uint8_t segment_size = message->read<uint8_t>();
        
            // If the segment size is 255+ check the next bit, a 0x01 indicates
            // over 255 and that the next bit should be added to the total
            // size. If 0x00 then the next bit should be skipped.
            if (segment_size == 0xFF) {
                if (message->read<uint8_t>() == 0x01) {
                    segment_size += message->read<uint8_t>();
                } else {
                    message->read<uint8_t>();
                }
            }
    
            std::tr1::shared_ptr<ByteBuffer> segment(new ByteBuffer(message->data()+message->readPosition(), segment_size));
		    session.handlePacket(segment);

            message->readPosition(message->readPosition() + segment_size);
        }        
    } else {
    
        std::tr1::shared_ptr<ByteBuffer> segment(new ByteBuffer(message->data()+message->readPosition(), message->size()-7));
		session.handlePacket(segment);
    }
}


void HandleDisconnect(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message)
{
	session.serverSequence(0);
	session.clientSequence(0);
	session.receivedSequence(0);
}


void HandlePing(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message)
{     
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\SOE\\PingResponse.txt");
    session.sendToRemote(packet, true, false, true);
}

