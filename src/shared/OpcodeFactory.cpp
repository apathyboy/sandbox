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

#include "OpcodeFactory.h"
#include "ByteBuffer.h"
#include "SOEHandler.h"
#include "LoginHandler.h"
#include "ZoneInsertionHandler.h"
#include "SpatialHandler.h"

MessageHandler OpcodeFactory::getOpcodeHandler(std::tr1::shared_ptr<ByteBuffer> packet)
{
	// Get the map of opcode handlers.
	MessageHandlers handlers = _buildMessageHandlerMap();

	// Prepare a container for the handler function.
	MessageHandler handler = NULL;

	MessageHandlers::iterator i = handlers.find(packet->peekAt<uint8_t>(1));

	if (i != handlers.end())
	{
		handler = (*i).second;
	}

	if (! handler)
	{
        i = handlers.find(packet->peekAt<uint32_t>(2));

        if (i != handlers.end()) {
            handler = (*i).second;
        } else {
            throw new OpcodeHandlerException();
        }
	}

	return handler;
}


MessageHandler OpcodeFactory::getOpcodeHandler(uint32_t opcode)
{
	// Get the map of opcode handlers.
	MessageHandlers handlers = _buildMessageHandlerMap();

	// Prepare a container for the handler function.
	MessageHandler handler = NULL;

	MessageHandlers::iterator i = handlers.find(opcode);

	if (i != handlers.end())
	{
		handler = (*i).second;
	}
	else 
	{
		throw new OpcodeHandlerException();
	}

	return handler;
}

MessageHandlers OpcodeFactory::_buildMessageHandlerMap()
{
	static MessageHandlers map;
	
	if (map.size() == 0)
	{
        map[1]	= std::tr1::bind(&HandleSessionRequest, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		/*map[3]	= &HandleMultiPacket;
		map[5]  = &HandleDisconnect;
		map[6]  = &HandlePing;
		map[7]	= &HandleNetStatus;
		map[9]  = &HandleDataChannel;
		map[21] = &HandleAcknowledge;

		map[0x41131F96] = &HandleAuthentication;
	
		map[0xD5899226] = &HandleSession;
		map[0xB5098D76] = &HandleZoneInsertionRequest;
		map[0x4C3D2CFA] = &HandleLoadTerrain;
		map[0x43FD1C22] = &HandleLoadDone;
		map[0xCA88FBAD] = &HandleClientReady;
		map[0x48f493c5] = &HandleClientReady;

		// Spatial
		map[0x80ce5e46] = &HandleSpatial;
		map[0xb719fa26] = &HandleSit;
		map[0xa8a25c79] = &HandleStand;
		map[0x7c8d63d4] = &HandleSpatialChat;
		map[0xbd8d02af] = &HandleProne;
		map[0x01b48b26] = &HandleKneel;
		map[0x7759F35E] = &HandleMood;
		map[0x32CF1BEE] = &HandleEmote;*/
	}

	return map;
}



handlerFunc OpcodeFactory::GetOpcodeHandler(std::tr1::shared_ptr<ByteBuffer> packet)
{
	// Get the map of opcode handlers.
	OpcodeHandlers handlers = _buildOpcodeHandlerMap();

	// Prepare a container for the handler function.
	handlerFunc handler = NULL;

	OpcodeHandlers::iterator i = handlers.find(packet->peekAt<uint8_t>(1));

	if (i != handlers.end())
	{
		handler = (*i).second;
	}

	if (! handler)
	{
        i = handlers.find(packet->peekAt<uint32_t>(2));

        if (i != handlers.end()) {
            handler = (*i).second;
        } else {
            throw new OpcodeHandlerException();
        }
	}

	return handler;
}


handlerFunc OpcodeFactory::GetOpcodeHandler(unsigned int* opcode)
{
	// Get the map of opcode handlers.
	OpcodeHandlers handlers = _buildOpcodeHandlerMap();

	// Prepare a container for the handler function.
	handlerFunc handler = NULL;

	OpcodeHandlers::iterator i = handlers.find(*opcode);

	if (i != handlers.end())
	{
		handler = (*i).second;
	}
	else 
	{
		throw new OpcodeHandlerException();
	}

	return handler;
}

OpcodeHandlers OpcodeFactory::_buildOpcodeHandlerMap()
{
	static OpcodeHandlers map;
	
	if (map.size() == 0)
	{
		map[1]	= &HandleSessionRequest;
		map[3]	= &HandleMultiPacket;
		map[5]  = &HandleDisconnect;
		map[6]  = &HandlePing;
		map[7]	= &HandleNetStatus;
		map[9]  = &HandleDataChannel;
		map[21] = &HandleAcknowledge;

		map[0x41131F96] = &HandleAuthentication;
	
		map[0xD5899226] = &HandleSession;
		map[0xB5098D76] = &HandleZoneInsertionRequest;
		map[0x4C3D2CFA] = &HandleLoadTerrain;
		map[0x43FD1C22] = &HandleLoadDone;
		map[0xCA88FBAD] = &HandleClientReady;
		map[0x48f493c5] = &HandleClientReady;

		// Spatial
		map[0x80ce5e46] = &HandleSpatial;
		map[0xb719fa26] = &HandleSit;
		map[0xa8a25c79] = &HandleStand;
		map[0x7c8d63d4] = &HandleSpatialChat;
		map[0xbd8d02af] = &HandleProne;
		map[0x01b48b26] = &HandleKneel;
		map[0x7759F35E] = &HandleMood;
		map[0x32CF1BEE] = &HandleEmote;
	}

	return map;
}
