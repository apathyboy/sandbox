/*
 * OpenSWG Combat Upgrade Sandbox - SpatialHandler.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef OPENSWG_SPATIAL_HANDLER_H
#define OPENSWG_SPATIAL_HANDLER_H

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <tr1/memory>

class Session;
class ByteBuffer;

void HandleSpatial(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandlePositionUpdate(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleSit(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleStand(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleProne(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleKneel(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleSpatialChat(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleMood(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleEmote(Session& session, std::tr1::shared_ptr<ByteBuffer> message);

#endif // OPENSWG_SPATIAL_HANDLER_H
