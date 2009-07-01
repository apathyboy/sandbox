/*
 * OpenSWG Combat Upgrade Sandbox - LoginServer.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "LoginServer.h"
#include "Logger.h"

LoginServer::LoginServer(uint16_t port)
: SocketServer(port)
{}

LoginServer::~LoginServer()
{}

void LoginServer::onUpdate()
{}