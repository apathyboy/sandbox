/*
 * OpenSWG Combat Upgrade Sandbox - LoginServer.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "LoginServer.h"
#include "Logger.h"
#include "SOEHandler.h"
#include "LoginHandler.h"
#include "Session.h"

LoginServer::LoginServer(uint16_t port)
    : SocketServer(port)
    , protocol_()
{}


LoginServer::~LoginServer()
{}

void LoginServer::onIncoming(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> message)
{}


void LoginServer::onUpdate()
{
}


void LoginServer::initializeProtocol()
{
    protocol_.addHandler(1, std::tr1::bind(&HandleSessionRequest, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(3, std::tr1::bind(&HandleMultiPacket, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(5,  std::tr1::bind(&HandleDisconnect, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(6,  std::tr1::bind(&HandlePing, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(7, std::tr1::bind(&HandleNetStatus, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(9,  std::tr1::bind(&HandleDataChannel, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    protocol_.addHandler(21, std::tr1::bind(&HandleAcknowledge, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    protocol_.addHandler(0x41131F96, std::tr1::bind(&HandleAuthentication, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    protocol_.addHandler(0xD5899226, std::tr1::bind(&HandleSession, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}

