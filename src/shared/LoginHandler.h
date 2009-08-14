/*
 * OpenSWG Combat Upgrade Sandbox - LoginHandler.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef OPENSWG_LOGIN_HANDLER_H
#define OPENSWG_LOGIN_HANDLER_H

class Session;
class ByteBuffer;

void HandleAuthentication(Session& session, ByteBuffer& message);
void HandleSession(Session& session, ByteBuffer& message);

#endif // OPENSWG_LOGIN_HANDLER_H