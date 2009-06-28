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

void HandleSessionRequest(GalaxySession *session, const unsigned char *data, unsigned short length)
{ 	
	// Store the connection id.
	session->connectionId(*(unsigned int*)(data+6));

	// Load in the raw packet data.
	unsigned short size;
	char *packet = loadPacket("packets\\SOE\\SessionResponse.txt", &size);

	// Add the connection id to the packet data.
	unsigned int *ptr	= (unsigned int*)(packet+2);
	*ptr = session->connectionId();

	// Add the crc seed to the packet data.
	ptr	 = (unsigned int*)(packet+6);
	*ptr = htonl(session->crcSeed());

	// Send out the packet.
	session->SendPacket(packet, size, false, false, false);

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

	// Load in the raw packet data.
	unsigned short size;
	char *packet = loadPacket("packets\\SOE\\NetStatus.txt", &size);

	// Insert the tick into the packet data.
	unsigned short *ptr = (unsigned short*)(packet+2);
	*ptr = tick;

	// Send out the packet.
	session->SendPacket(packet, size, true, true, false);
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
		session->HandlePacket(buffer);

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
			session->HandlePacket(buffer);
            i+=segment_size;
            data+=segment_size;
        }
    }
    
	else
    {
		unsigned short segment_size = length-4;
        std::tr1::shared_ptr<ByteBuffer> buffer(new ByteBuffer(data, segment_size));
		session->HandlePacket(buffer);
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
	// Load in the raw packet data.
	unsigned short size;
	char *packet = loadPacket("packets\\SOE\\PingResponse.txt", &size);

	// Send out the packet.
	session->SendPacket(packet, size, true, false, true);
}