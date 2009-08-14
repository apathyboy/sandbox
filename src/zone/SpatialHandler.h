/*
 * OpenSWG Combat Upgrade Sandbox - SpatialHandler.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef OPENSWG_SPATIAL_HANDLER_H
#define OPENSWG_SPATIAL_HANDLER_H

class Session;
class ByteBuffer;

void HandleSpatial(Session& session, ByteBuffer& message);
void HandlePositionUpdate(Session& session, ByteBuffer& message);
void HandleSit(Session& session, ByteBuffer& message);
void HandleStand(Session& session, ByteBuffer& message);
void HandleProne(Session& session, ByteBuffer& message);
void HandleKneel(Session& session, ByteBuffer& message);
void HandleSpatialChat(Session& session, ByteBuffer& message);
void HandleMood(Session& session, ByteBuffer& message);
void HandleEmote(Session& session, ByteBuffer& message);

#endif // OPENSWG_SPATIAL_HANDLER_H
