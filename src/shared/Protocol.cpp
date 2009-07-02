/*
 * OpenSWG Combat Upgrade Sandbox - Protocol.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include <exception>

#include "Protocol.h"
#include "ByteBuffer.h"

Protocol::Protocol()
{}


Protocol::~Protocol()
{}


void Protocol::addHandler(uint32_t id, Protocol::PacketHandler handler)
{
    message_handlers_.insert(std::make_pair<uint32_t, PacketHandler>(id, handler));
}


Protocol::PacketHandler Protocol::find(uint32_t id)
{
    PacketHandler handler = NULL;

    PacketHandlerMap::iterator i = message_handlers_.find(id);
    if (i != message_handlers_.end()) {
        handler = (*i).second;
    }

    return handler;
}

Protocol::PacketHandler Protocol::find(std::tr1::shared_ptr<ByteBuffer> packet)
{
	PacketHandler handler = NULL;

	PacketHandlerMap::iterator i = message_handlers_.find(packet->peekAt<uint8_t>(1));

	if (i != message_handlers_.end())
	{
		handler = (*i).second;
	}

	if (! handler)
	{
        i = message_handlers_.find(packet->peekAt<uint32_t>(2));

        if (i != message_handlers_.end()) {
            handler = (*i).second;
        } else {
            throw std::exception("Unidentified Packet");
        }
	}

	return handler;
}




