/*
 * OpenSWG Combat Upgrade Sandbox - SOEHandler.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef OPENSWG_SOE_HANDLER_H
#define OPENSWG_SOE_HANDLER_H

#include <tr1/memory>

class Session;
class ByteBuffer;

void HandleSessionRequest(Session& session, ByteBuffer& message);
void HandleNetStatus(Session& session, ByteBuffer& message);
void HandleMultiPacket(Session& session, ByteBuffer& message);
void HandleAcknowledge(Session& session, ByteBuffer& message);
void HandleDataChannel(Session& session, ByteBuffer& message);
void HandleDisconnect(Session& session, ByteBuffer& message);
void HandlePing(Session& session, ByteBuffer& message);

#endif // OPENSWG_SOE_HANDLER_H