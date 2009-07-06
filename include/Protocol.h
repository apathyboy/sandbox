/*
 * OpenSWG Combat Upgrade Sandbox - Protocol.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <map>
#include <tr1/functional>
#include <tr1/memory>

class ByteBuffer;
class Session;

typedef std::tr1::function<void (Session& session, std::tr1::shared_ptr<ByteBuffer>)> MessageHandler;

//template<typename Identifier, class Handler = MessageHandler>
class Protocol
{
public:
    Protocol();
    ~Protocol();

    void addHandler(uint32_t id, MessageHandler handler);
    
    MessageHandler find(uint32_t id);
    MessageHandler find(std::tr1::shared_ptr<ByteBuffer> message);

private:
    Protocol(const Protocol&);
    Protocol& operator=(const Protocol&);

    typedef std::map<uint32_t, MessageHandler> MessageHandlerMap;
    MessageHandlerMap message_handlers_;
};


#endif // PROTOCOL_H_

