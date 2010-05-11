/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SRC_ZONE_SPATIAL_HANDLER_H_
#define SRC_ZONE_SPATIAL_HANDLER_H_

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

#endif // SRC_ZONE_SPATIAL_HANDLER_H_
