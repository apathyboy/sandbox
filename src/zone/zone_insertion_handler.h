/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SRC_ZONE_ZONE_INSERTION_HANDLER_H_
#define SRC_ZONE_ZONE_INSERTION_HANDLER_H_

namespace sandbox {

namespace shared {

class Session;
class ByteBuffer;

}  // namespace shared

namespace zone {

void HandleClientReady(shared::Session& session, shared::ByteBuffer& message);
void HandleLoadDone(shared::Session& session, shared::ByteBuffer& message);
void HandleLoadTerrain(shared::Session& session, shared::ByteBuffer& message);
void HandleZoneInsertionRequest(shared::Session& session,
                                shared::ByteBuffer& message);


}  // namespace zone
}  // namespace sandbox

#endif  // SRC_ZONE_ZONE_INSERTION_HANDLER_H_
