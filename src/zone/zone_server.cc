/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include "zone/zone_server.h"
#include "zone/spatial_handler.h"
#include "zone/zone_insertion_handler.h"
#include "shared/logger.h"
#include "shared/session.h"
#include "shared/login_handler.h"

namespace sandbox {
namespace zone {

ZoneServer::ZoneServer(uint16_t port)
: shared::GalaxyServer(port) {
    shared::Logger().log(shared::INFO)
      << "Zone Server listening on port: [" << port << "]";
}


ZoneServer::~ZoneServer() {}


void ZoneServer::onIncoming(const shared::NetworkAddress& address,
                            shared::ByteBuffer& message) {}


void ZoneServer::onUpdate() {}


void ZoneServer::initializeProtocol() {
  addSwgProtocolHandler(0x41131F96,
    std::tr1::bind(&shared::HandleAuthentication,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));

  addSwgProtocolHandler(0xD5899226,
    std::tr1::bind(&shared::HandleSession,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));

  addSwgProtocolHandler(0xB5098D76,
    std::tr1::bind(&HandleZoneInsertionRequest,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0x4C3D2CFA,
    std::tr1::bind(&HandleLoadTerrain,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0x43FD1C22,
    std::tr1::bind(&HandleLoadDone,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0xCA88FBAD,
    std::tr1::bind(&HandleClientReady,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0x48f493c5,
    std::tr1::bind(&HandleClientReady,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));

  // Spatial
  addSwgProtocolHandler(0x80ce5e46,
    std::tr1::bind(&HandleSpatial,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0xb719fa26,
    std::tr1::bind(&HandleSit,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0xa8a25c79,
    std::tr1::bind(&HandleStand,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0x7c8d63d4,
    std::tr1::bind(&HandleSpatialChat,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0xbd8d02af,
    std::tr1::bind(&HandleProne,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0x01b48b26,
    std::tr1::bind(&HandleKneel,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0x7759F35E,
    std::tr1::bind(&HandleMood,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  addSwgProtocolHandler(0x32CF1BEE,
    std::tr1::bind(&HandleEmote,
      std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}

}  // namespace zone
}  // namespace sandbox
