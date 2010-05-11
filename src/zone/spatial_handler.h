/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SANDBOX_ZONE_SPATIAL_HANDLER_H_
#define SANDBOX_ZONE_SPATIAL_HANDLER_H_

namespace sandbox {

namespace shared {

class Session;
class ByteBuffer;

}  // namespace shared

namespace zone {

void HandleSpatial(shared::Session& session, shared::ByteBuffer& message);
void HandlePositionUpdate(shared::Session& session, shared::ByteBuffer& message);
void HandleSit(shared::Session& session, shared::ByteBuffer& message);
void HandleStand(shared::Session& session, shared::ByteBuffer& message);
void HandleProne(shared::Session& session, shared::ByteBuffer& message);
void HandleKneel(shared::Session& session, shared::ByteBuffer& message);
void HandleSpatialChat(shared::Session& session, shared::ByteBuffer& message);
void HandleMood(shared::Session& session, shared::ByteBuffer& message);
void HandleEmote(shared::Session& session, shared::ByteBuffer& message);

}  // namespace zone
}  // namespace sandbox

#endif // SANDBOX_ZONE_SPATIAL_HANDLER_H_
