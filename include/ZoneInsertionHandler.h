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

void HandleClientReady(Session& session, ByteBuffer& message);
void HandleLoadDone(Session& session, ByteBuffer& message);
void HandleLoadTerrain(Session& session, ByteBuffer& message);
void HandleZoneInsertionRequest(Session& session, ByteBuffer& message);

#endif // OPENSWG_ZONE_INSERTION_HANDLER_H