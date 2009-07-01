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

void HandleSessionRequest(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleNetStatus(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleMultiPacket(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleAcknowledge(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleDataChannel(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleDisconnect(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandlePing(Session& session, std::tr1::shared_ptr<ByteBuffer> message);

#endif // OPENSWG_SOE_HANDLER_H