/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef OPENSWG_ZONE_INSERTION_HANDLER_H
#define OPENSWG_ZONE_INSERTION_HANDLER_H

class Session;
class ByteBuffer;

void HandleClientReady(Session& session, ByteBuffer& message);
void HandleLoadDone(Session& session, ByteBuffer& message);
void HandleLoadTerrain(Session& session, ByteBuffer& message);
void HandleZoneInsertionRequest(Session& session, ByteBuffer& message);

#endif // OPENSWG_ZONE_INSERTION_HANDLER_H