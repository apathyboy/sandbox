/*
 * OpenSWG Combat Upgrade Sandbox - OpcodeFactory.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

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
		map[3]	= std::tr1::bind(&HandleMultiPacket, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[5]  = std::tr1::bind(&HandleDisconnect, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[6]  = std::tr1::bind(&HandlePing, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[7]	= std::tr1::bind(&HandleNetStatus, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[9]  = std::tr1::bind(&HandleDataChannel, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[21] = std::tr1::bind(&HandleAcknowledge, std::tr1::placeholders::_1, std::tr1::placeholders::_2);

		map[0x41131F96] = std::tr1::bind(&HandleAuthentication, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
	
		map[0xD5899226] = std::tr1::bind(&HandleSession, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0xB5098D76] = std::tr1::bind(&HandleZoneInsertionRequest, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0x4C3D2CFA] = std::tr1::bind(&HandleLoadTerrain, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0x43FD1C22] = std::tr1::bind(&HandleLoadDone, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0xCA88FBAD] = std::tr1::bind(&HandleClientReady, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0x48f493c5] = std::tr1::bind(&HandleClientReady, std::tr1::placeholders::_1, std::tr1::placeholders::_2);

		// Spatial
		map[0x80ce5e46] = std::tr1::bind(&HandleSpatial, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0xb719fa26] = std::tr1::bind(&HandleSit, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0xa8a25c79] = std::tr1::bind(&HandleStand, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0x7c8d63d4] = std::tr1::bind(&HandleSpatialChat, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0xbd8d02af] = std::tr1::bind(&HandleProne, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0x01b48b26] = std::tr1::bind(&HandleKneel, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0x7759F35E] = std::tr1::bind(&HandleMood, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
		map[0x32CF1BEE] = std::tr1::bind(&HandleEmote, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
	}

	return map;
}

