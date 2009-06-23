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

#include "SpatialHandler.h"
#include "GalaxySession.h"
#include "OpcodeFactory.h"
#include "Logger.h"
#include "PacketTools.h"

void HandleSpatial(GalaxySession *session, char *data, unsigned short length)
{	
    unsigned int *opcode = (unsigned int*)(data+30);
	// Try to handle the incoming packet.
	try
	{
		// Search for the opcode handler function and pass it the packet data.
		handlerFunc handler = OpcodeFactory::GetOpcodeHandler(opcode);
		handler(session, data, length);
	}
	catch(...)
	{
		// Log any unknown opcodes.
		Logger().log(ERR) << "Unknown Spatial Opcode Found: " << *(opcode);	
		session->SendOk();
	}
}


/** The body of this function was written by SWGEmu. It may be replaced
 *	in the future to better integrate with the rest of the OpenSWG
 *	code.
 *	Copyright (C) 2006 Team SWGEmu <http://www.swgemu.com>
 */
void HandlePositionUpdate(GalaxySession *session, char *data, unsigned short length)
{
	signed short direction = *(signed short*)(data+44);

	float x = *(float*)(data+46);
	float y = *(float*)(data+50);
	float z = *(float*)(data+54);

	signed short a, b, c;

	a = (int)((x - 1)*4);
	b = (int)((y - 1)*4);
	c = (int)(z * 4);
	
	// Load in the raw packet data.
	unsigned short size;
	char *packet = loadPacket("packets\\Spatial\\PositionUpdate.txt", &size);

	memcpy(packet+14, &a, 2);
	memcpy(packet+16, &c, 2);
	memcpy(packet+18, &b, 2);

	a = *(unsigned short*)(packet+20)+1;
	b = 0;
	direction = direction / 4;

	memcpy(packet+20, &a, 2);
	memcpy(packet+22, &b, 2);
	memcpy(packet+24, &direction, 2);

	
	*(unsigned short*)(packet+2) = (unsigned short)htons(session->GetServerSequence());
//	session->SendPacket(packet, size, true, false, true);

	//delete [] pData;
//	session->IncrementServerSequence();	
}

void HandleSit(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();

	session->SendHardPacket("packets\\Spatial\\SelfPostureUpdate_Sit.txt", false);
	session->SendHardPacket("packets\\Spatial\\PostureUpdate_Sit.txt", false);
}

void HandleStand(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();

	session->SendHardPacket("packets\\Spatial\\SelfPostureUpdate_Stand.txt", false);
	session->SendHardPacket("packets\\Spatial\\PostureUpdate_Stand.txt", false);
}

void HandleProne(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();

	session->SendHardPacket("packets\\Spatial\\SelfPostureUpdate_Prone.txt", false);
	session->SendHardPacket("packets\\Spatial\\PostureUpdate_Prone.txt",false);
}

void HandleKneel(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();

	session->SendHardPacket("packets\\Spatial\\SelfPostureUpdate_Kneel.txt", false);
	session->SendHardPacket("packets\\Spatial\\PostureUpdate_Kneel.txt",false);

}

void HandleSpatialChat(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();

	data += 42;
	uint32_t textsize = *(uint32_t*)(data);
	data+=4;

	char moods[5][32];
    memset(moods[0], 0, 32);
    memset(moods[1], 0, 32);
    memset(moods[2], 0, 32);
    memset(moods[3], 0, 32);
    memset(moods[4], 0, 32);
	
	uint8_t spaces = 0;
	uint8_t j = 0;
    uint32_t lenRemove = 0;
	while (spaces < 5) {
		if (*(uint8_t*)data == 32) {
			spaces++;
			data+=2;
			j=0;
			continue;
		}

		if (*(uint8_t*)data == 0) {
            lenRemove++;
			data++;
			continue;
		}

		moods[spaces][j] = *(uint8_t*)data;
		j++;
		data++;
        lenRemove++;
	}
            
	lenRemove /= 2;            
	lenRemove += 5;	
	textsize -= lenRemove;
	textsize *= 2;

	uint64_t moodId[5];
    moodId[0] = atoi(moods[0]); //Target.            
	moodId[1] = atoi(moods[1]);            
	moodId[2] = atoi(moods[2]);            
	moodId[3] = atoi(moods[3]);            
	moodId[4] = atoi(moods[4]);

	if (moodId[2] == 0)
		moodId[2] = (uint64_t)session->GetPlayer()->GetMood();
	
	wchar_t *U_text = new wchar_t[(textsize)];
	memcpy(U_text, data, textsize);

	session->SendText(U_text, textsize, moodId);
}


void HandleMood(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();

	data += 42;
	uint32_t size = *(uint32_t*)(data);
	data+=4;
           
	char * moodString = new char[size+1];
    memset(moodString, 0, size+1);
    for (uint32_t i = 0; i<size; i++)
    {
	    if (*data == 0 || *data == 32) {
		    break;
        }
        
		moodString[i] = *data;
		data+=2;
	}
    
	session->GetPlayer()->SetMood(atoi(moodString));

	unsigned short packetSize;
	char *packet = loadPacket("packets\\ZoneInsertion\\Creo6.txt", &packetSize);

	*(uint16_t *)(packet+99) = session->GetPlayer()->GetMood();

	session->SendHardPacket(packet, packetSize, true);

    delete [] moodString;
}

void HandleEmote(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();

	data += 42;
	uint32_t size = *(uint32_t*)(data);
	data+=4; 
	uint16_t emoteId=0x00;
    //uint16 aux;
    char *emoteString = new char[size+1];
    memset(emoteString, 0, size+1);
    bool spFound = 0;
    int b=0;
    for (unsigned int i=0;i<size;i++) {
	    if (spFound) {
		    if (*data == 32) {
			    emoteString[b] = 0;
                break;
            }
            
			emoteString[b] = *data;
            b++;
        }
        else {
            if (*data == 32) {
				spFound = 1;
			}
        }
		data +=2;
    }      

    emoteId = atoi(emoteString);
	delete [] emoteString;

	unsigned short packetSize;
	char *packet = loadPacket("packets\\Spatial\\PlayerEmote.txt", &packetSize);

	*(uint16_t *)(packet+46) = emoteId;

	session->SendHardPacket(packet, packetSize, true);
}