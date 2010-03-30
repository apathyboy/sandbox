/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include "LoginServer.h"
#include "Logger.h"
#include "LoginHandler.h"
#include "Session.h"

LoginServer::LoginServer(uint16_t port)
    : GalaxyServer(port)
{
    Logger().log(INFO) << "Login Server listening on port: [" << port << "]";
}


LoginServer::~LoginServer()
{}

void LoginServer::onIncoming(const NetworkAddress& address, ByteBuffer& message)
{}


void LoginServer::onUpdate()
{
}


void LoginServer::initializeProtocol()
{
    addSwgProtocolHandler(0x41131F96, std::tr1::bind(&HandleAuthentication, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    addSwgProtocolHandler(0xD5899226, std::tr1::bind(&HandleSession, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}

