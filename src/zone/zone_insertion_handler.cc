/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include "zone/zone_insertion_handler.h"
#include "shared/session.h"
#include "shared/packet_tools.h"

namespace sandbox {
namespace zone {

void HandleClientReady(shared::Session& session, shared::ByteBuffer& message) {
  session.sendHeartbeat();
}


void HandleLoadDone(shared::Session& session, shared::ByteBuffer& message) {
  session.sendHardcodedPacket("packets/Zone/LoadDone.txt", true);
}


void HandleLoadTerrain(shared::Session& session, shared::ByteBuffer& message) {
  session.sendHeartbeat();
}


void HandleZoneInsertionRequest(shared::Session& session,
                                shared::ByteBuffer& message) {
  session.sendHeartbeat();

  // Send the initial batch of packets.
  session.sendHardcodedPacket(
    "packets/Zone/ChatServerStatus.txt", false);
  session.sendHardcodedPacket(
    "packets/Zone/ParametersMessage.txt", false);
  session.sendHardcodedPacket(
    "packets/Zone/CmdSceneStart.txt", true);

  // Begin loading the character.
  session.sendHardcodedPacket(
    "packets/Zone/UpdatePvpStatusMessage.txt", false);
  session.sendHardcodedPacket(
    "packets/Zone/SceneCreateObjectByCrc.txt", false);

  session.sendHardcodedPacket(
    "packets/Zone/Baselines/Creo1.txt", true);
  session.sendHardcodedPacket(
    "packets/Zone/Baselines/Creo3.txt", true);
  session.sendHardcodedPacket(
    "packets/Zone/Baselines/Creo6.txt", true);

  // @TODO Implement a better way to send hardcoded fragmented packets.
  session.sendHardcodedPacket("packets/Zone/Creo4.txt", true);
  session.sendHardcodedPacket("packets/Zone/Creo4pt2.txt", true);
  session.sendHardcodedPacket("packets/Zone/Creo4pt3.txt", true);
  session.sendHardcodedPacket("packets/Zone/Creo4pt4.txt", true);

  // Create our character.
  session.sendHardcodedPacket("packets/Zone/Creo8.txt", true);
  session.sendHardcodedPacket("packets/Zone/Play8.txt", true);
  session.sendHardcodedPacket("packets/Zone/Play9.txt", false);
  session.sendHardcodedPacket("packets/Zone/PrecloseClose.txt", false);
  session.sendHardcodedPacket("packets/Zone/PlayClose.txt", false);
  session.sendHardcodedPacket("packets/Zone/Play9Update.txt", false);

  // Inventory
  session.sendHardcodedPacket("packets/Zone/Inventory.txt", false);

  // Environment
  session.sendHardcodedPacket("packets/Objects/KerenStarship.txt", false);
  session.sendHardcodedPacket(
    "packets/Objects/KerenStarshipTicketCollector.txt", false);
}

}  // namespace zone
}  // namespace sandbox
