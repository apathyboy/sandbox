/*
 * OpenSWG: GalaxyEngine - SoeMessageFactory.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef SOE_MESSAGE_FACTORY_H_
#define SOE_MESSAGE_FACTORY_H_

#ifdef _MSC_VER
#include "stdint.h"
#include <memory>
#else
#include <cstdint>
#include <tr1/memory>
#endif

class ByteBuffer;
class Session;
class GalaxyServer;

class SoeMessageFactory
{
public:
    static std::tr1::shared_ptr<ByteBuffer> buildSessionResponse(const GalaxyServer& server, std::tr1::shared_ptr<Session> session);
    static std::tr1::shared_ptr<ByteBuffer> buildNetworkStatusResponse(std::tr1::shared_ptr<Session> session, uint16_t tick);
    static std::tr1::shared_ptr<ByteBuffer> buildKeepAliveResponse();

private:
	SoeMessageFactory();
	~SoeMessageFactory();
    SoeMessageFactory(const SoeMessageFactory&);
    SoeMessageFactory& operator=(const SoeMessageFactory&);
};

#endif // SOE_MESSAGE_FACTORY_H_

