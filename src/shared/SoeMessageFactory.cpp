/*
 * OpenSWG: GalaxyEngine - SoeMessageFactory.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "SoeMessageFactory.h"
#include "ByteBuffer.h"
#include "GalaxyServer.h"
#include "Session.h"

std::tr1::shared_ptr<ByteBuffer> SoeMessageFactory::buildSessionResponse(const GalaxyServer& server, std::tr1::shared_ptr<Session> session)
{
    std::tr1::shared_ptr<ByteBuffer> session_response(new ByteBuffer());
    
    *session_response << htons(0x0002);
    *session_response << session->connectionId();
    *session_response << htonl(session->crcSeed());
    *session_response << static_cast<uint8_t>(session->crcLength());
    *session_response << htons(server.encryptionMethod()); 
    *session_response << htonl(server.maxUdpSize());  

    return session_response;
}


std::tr1::shared_ptr<ByteBuffer> SoeMessageFactory::buildNetworkStatusResponse(std::tr1::shared_ptr<Session> session, uint16_t tick)
{
    std::tr1::shared_ptr<ByteBuffer> network_status_response(new ByteBuffer());

    *network_status_response << htons(0x0008);
    *network_status_response << tick; // Client Tick Count
   
    for (int8_t i = 0; i < 39; ++i) {
        *network_status_response << static_cast<uint8_t>(0);
    }

    return network_status_response;
}


std::tr1::shared_ptr<ByteBuffer> SoeMessageFactory::buildKeepAliveResponse()
{
    std::tr1::shared_ptr<ByteBuffer> keep_alive_response(new ByteBuffer());

    *keep_alive_response << htons(0x0006);
    *keep_alive_response << 0x00 << 0x0000; // Footer

    return keep_alive_response;
}

