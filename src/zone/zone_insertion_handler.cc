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
  session.sendHardcodedPacket("packets/ZoneInsertion/LoadDone.txt", true);
}


void HandleLoadTerrain(shared::Session& session, shared::ByteBuffer& message) {
  session.sendHeartbeat();
}


void HandleZoneInsertionRequest(shared::Session& session,
                                shared::ByteBuffer& message) {
  session.sendHeartbeat();

  // Send the initial batch of packets.
  session.sendHardcodedPacket("packets/ZoneInsertion/UnkByteFlag.txt", false);
  session.sendHardcodedPacket("packets/ZoneInsertion/Int900Pkt.txt", false);
  session.sendHardcodedPacket("packets/ZoneInsertion/LoadTerrain.txt", true);

  // Begin loading the character.
  session.sendHardcodedPacket(
    "packets/ZoneInsertion/PreloadCharacter.txt", false);
  session.sendHardcodedPacket("packets/ZoneInsertion/CreoCreate.txt", false);
  session.sendHardcodedPacket("packets/ZoneInsertion/Creo3.txt", true);
  session.sendHardcodedPacket("packets/ZoneInsertion/Creo6.txt", true);

  session.sendHardcodedPacket("packets/ZoneInsertion/Creo1.txt", true);

  // @TODO Implement a better way to send hardcoded fragmented packets.
  session.sendHardcodedPacket("packets/ZoneInsertion/Creo4.txt", true);
  session.sendHardcodedPacket("packets/ZoneInsertion/Creo4pt2.txt", true);
  session.sendHardcodedPacket("packets/ZoneInsertion/Creo4pt3.txt", true);
  session.sendHardcodedPacket("packets/ZoneInsertion/Creo4pt4.txt", true);

  // Create our character.
  session.sendHardcodedPacket("packets/ZoneInsertion/Creo8.txt", true);
  session.sendHardcodedPacket("packets/ZoneInsertion/Play8.txt", true);
  session.sendHardcodedPacket("packets/ZoneInsertion/Play9.txt", false);
  session.sendHardcodedPacket("packets/ZoneInsertion/PrecloseClose.txt", false);
  session.sendHardcodedPacket("packets/ZoneInsertion/PlayClose.txt", false);
  session.sendHardcodedPacket("packets/ZoneInsertion/Play9Update.txt", false);

  // Inventory
  session.sendHardcodedPacket("packets/ZoneInsertion/Inventory.txt", false);

  // Environment
  session.sendHardcodedPacket("packets/Objects/KerenStarship.txt", false);
  session.sendHardcodedPacket(
    "packets/Objects/KerenStarshipTicketCollector.txt", false);
}

}  // namespace zone
}  // namespace sandbox
