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
#include <functional>
#include <memory>
#else
#include <cstdint>
#include <tr1/functional>
#include <tr1/memory>
#endif

#include <map>

#include "ByteBuffer.h"

class Session;

typedef std::tr1::function<void (Session& session, ByteBuffer&)> MessageHandler;

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

        typename std::map<Identifier, Handler>::iterator i = handlers_.find(id);
        if (i != handlers_.end()) {
            handler = (*i).second;
        }

        return handler;
    }

    Handler find(ByteBuffer& message)
    {
	    Handler handler = NULL;

	    typename std::map<Identifier, Handler>::iterator i = handlers_.find(message.peekAt<uint8_t>(1));

	    if (i != handlers_.end())
	    {
		    handler = (*i).second;
	    }

	    if (! handler)
	    {
            i = handlers_.find(message.peekAt<uint32_t>(2));

            if (i != handlers_.end()) {
                handler = (*i).second;
            } else {
                throw std::exception();
            }
	    }

	    return handler;
    }

private:
    Protocol(const Protocol&);
    Protocol& operator=(const Protocol&);

    std::map<Identifier, Handler> handlers_;
};


#endif // PROTOCOL_H_

