/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
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

