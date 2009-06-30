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
    //"Multi-SOE Packet: "
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
}


void HandleAcknowledge(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message)
{	
    message->read<uint16_t>();

    session.receivedSequence(message->read<uint16_t>());
    session.clientSequence(message->read<uint16_t>());
}




void HandleSessionRequest(GalaxySession *session, const unsigned char *data, unsigned short length)
{ 	
	// Store the connection id.
	session->connectionId(*(unsigned int*)(data+6));

    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\SOE\\SessionResponse.txt");

    packet->writeAt<uint32_t>(2, session->connectionId());
    packet->writeAt<uint32_t>(6, htonl(session->crcSeed()));

    session->sendToRemote(packet, false, false, false);

	// Send the connection packet.
	if (session->server()->port() == 44453) 
		session->sendHardcodedPacket("packets\\SOE\\LoginServer.txt", false);		
	else
		session->sendHardcodedPacket("packets\\SOE\\ConnectionServer.txt", false);
}

void HandleNetStatus(GalaxySession *session, const unsigned char *data, unsigned short length)
{     
	// Get the tick value.
	unsigned short tick  = *(unsigned short*)(data+2);
    
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\SOE\\NetStatus.txt");

    packet->writeAt<uint16_t>(2, tick);


    session->sendToRemote(packet, true, true, false);
}

void HandleMultiPacket(GalaxySession *session, const unsigned char *data, unsigned short length)
{       
    //"Multi-SOE Packet: "
    data+=2;
    for(short i = 2; i < length-4; i++)
    {
		unsigned short segment_size = 0;
        if((unsigned char)data[0] == 0xFF)
        {
            segment_size += 255;
            data++;
            i++;
            unsigned char counter = (unsigned char)*data;
            for(short c = 0; c < counter; c++)
				data++;
            
			i++;
            segment_size += (unsigned char)*data;
		}
        
		else
	        segment_size += (unsigned char)*data;
    
		data++;
	    i++;
    
        std::tr1::shared_ptr<ByteBuffer> buffer(new ByteBuffer(data, segment_size));
		session->handlePacket(buffer);

        i+=segment_size;
        data+=segment_size;
	}	
}

void HandleAcknowledge(GalaxySession *session, const unsigned char *data, unsigned short length)
{	
	session->receivedSequence(*(unsigned short*)(data+2));

    //you can add a check here *shrug*
    //if(oclient.seq_recv == oclient.server_sequence)
    //its ok, if not, re-request packet?
    data+=2;
	session->clientSequence(*(unsigned short*)(data));
}

void HandleDataChannel(GalaxySession *session, const unsigned char *data, unsigned short length)
{	
	data+=2;
	session->clientSequence(*(unsigned short*)(data));
    
	session->sendAcknowledge();

	data+=2;
    if(data[0] == 0x00 && data[1] == 0x19)
    {
        data+=2;
        for(short i =0;i < length-6;i++)
        {
            unsigned short segment_size = 0;
            if((unsigned char)data[0] == 0xFF)
            {
                segment_size += 255;
                data++;
                i++;
                unsigned char counter = *(unsigned char*)data;
                for(short c = 0;c < counter;c++)
                data+=1;
                i++;
                segment_size += (unsigned char)*data;
            }
            else
            {
	            segment_size += *(unsigned char*)data;
            }
            data++;
            i++;
            std::tr1::shared_ptr<ByteBuffer> buffer(new ByteBuffer(data, segment_size));
			session->handlePacket(buffer);
            i+=segment_size;
            data+=segment_size;
        }
    }
    
	else
    {
		unsigned short segment_size = length-4;
        std::tr1::shared_ptr<ByteBuffer> buffer(new ByteBuffer(data, segment_size));
		session->handlePacket(buffer);
    }
}

void HandleDisconnect(GalaxySession *session, const unsigned char *data, unsigned short length)
{
	session->serverSequence(0);
	session->clientSequence(0);
	session->receivedSequence(0);
}

void HandlePing(GalaxySession *session, const unsigned char *data, unsigned short length)
{     
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\SOE\\PingResponse.txt");
    session->sendToRemote(packet, true, false, true);
}