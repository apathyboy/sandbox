/*
 * Legacy Sandbox Project - Protocol.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "Protocol.h"

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




