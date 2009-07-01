/*
 * OpenSWG Combat Upgrade Sandbox - SOEHandler.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "SOEHandler.h"
#include "Session.h"
#include "PacketTools.h"
#include "Logger.h"

void HandleSessionRequest(Session& session, std::tr1::shared_ptr<ByteBuffer> message)
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


void HandleNetStatus(Session& session, std::tr1::shared_ptr<ByteBuffer> message)
{     
    uint32_t tick = message->peekAt<uint32_t>(2);
    
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\SOE\\NetStatus.txt");

    packet->writeAt<uint16_t>(2, tick);

    session.sendToRemote(packet, true, true, false);
}


void HandleMultiPacket(Session& session, std::tr1::shared_ptr<ByteBuffer> message)
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


void HandleAcknowledge(Session& session, std::tr1::shared_ptr<ByteBuffer> message)
{	
    message->read<uint16_t>();

    session.receivedSequence(message->read<uint16_t>());
    session.clientSequence(message->read<uint16_t>());
}


void HandleDataChannel(Session& session, std::tr1::shared_ptr<ByteBuffer> message)
{	
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


void HandleDisconnect(Session& session, std::tr1::shared_ptr<ByteBuffer> message)
{
	session.serverSequence(0);
	session.clientSequence(0);
	session.receivedSequence(0);
}


void HandlePing(Session& session, std::tr1::shared_ptr<ByteBuffer> message)
{     
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\SOE\\PingResponse.txt");
    session.sendToRemote(packet, true, false, true);
}

