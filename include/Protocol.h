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

template<typename Identifier, class Handler = MessageHandler>
class Protocol
{
public:
    Protocol()  {}
    ~Protocol() {}

    void addHandler(Identifier id, Handler handler)
    {
        handlers_.insert(std::make_pair<Identifier, Handler>(id, handler));
    }

    Handler find(Identifier id)
    {
        Handler handler = NULL;

        HandlerMap::iterator i = handlers_.find(id);
        if (i != handlers_.end()) {
            handler = (*i).second;
        }

        return handler;
    }

    Handler find(std::tr1::shared_ptr<ByteBuffer> message)
    {
	    Handler handler = NULL;

	    HandlerMap::iterator i = handlers_.find(message->peekAt<uint8_t>(1));

	    if (i != handlers_.end())
	    {
		    handler = (*i).second;
	    }

	    if (! handler)
	    {
            i = handlers_.find(message->peekAt<uint32_t>(2));

            if (i != handlers_.end()) {
                handler = (*i).second;
            } else {
                throw std::exception("Unidentified Packet");
            }
	    }

	    return handler;
    }

private:
    Protocol(const Protocol&);
    Protocol& operator=(const Protocol&);

    typedef std::map<Identifier, Handler> HandlerMap;
    HandlerMap handlers_;
};


#endif // PROTOCOL_H_

