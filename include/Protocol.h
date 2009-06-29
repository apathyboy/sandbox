/*
 * Legacy Sandbox Project - Protocol.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <map>
#include <tr1/functional>
#include <tr1/memory>

#include "ByteBuffer.h"
#include "GalaxySession.h"

typedef std::tr1::function<void (std::tr1::shared_ptr<GalaxySession>, std::tr1::shared_ptr<ByteBuffer>)> MessageHandler;

class Protocol
{
    Protocol();
    ~Protocol();

    void addHandler(uint32_t id, MessageHandler handler);
    
    MessageHandler find(uint32_t id);

private:
    Protocol(const Protocol&);
    Protocol& operator=(const Protocol&);

    typedef std::map<uint32_t, MessageHandler> MessageHandlerMap;
    MessageHandlerMap message_handlers_;
};


#endif // PROTOCOL_H_

