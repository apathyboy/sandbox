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


void Protocol::addHandler(uint32_t id, MessageHandler handler)
{
    message_handlers_.insert(std::make_pair<uint32_t, MessageHandler>(id, handler));
}


MessageHandler Protocol::find(uint32_t id)
{
    MessageHandler handler = NULL;

    MessageHandlerMap::iterator i = message_handlers_.find(id);
    if (i != message_handlers_.end()) {
        handler = (*i).second;
    }

    return handler;
}

MessageHandler Protocol::find(std::tr1::shared_ptr<ByteBuffer> packet)
{
	MessageHandler handler = NULL;

	MessageHandlerMap::iterator i = message_handlers_.find(packet->peekAt<uint8_t>(1));

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




