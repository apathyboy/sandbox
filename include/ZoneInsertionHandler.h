/*
 * OpenSWG Combat Upgrade Sandbox - ZoneInsertionHandler.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef OPENSWG_ZONE_INSERTION_HANDLER_H
#define OPENSWG_ZONE_INSERTION_HANDLER_H

#include <tr1/memory>

class Session;
class ByteBuffer;

void HandleClientReady(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleLoadDone(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleLoadTerrain(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleSession(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleZoneInsertionRequest(Session& session, std::tr1::shared_ptr<ByteBuffer> message);

#endif // OPENSWG_ZONE_INSERTION_HANDLER_H