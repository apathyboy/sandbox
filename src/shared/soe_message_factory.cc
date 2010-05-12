/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include "shared/soe_message_factory.h"

#include "shared/byte_buffer.h"
#include "shared/galaxy_server.h"
#include "shared/session.h"

namespace sandbox {
namespace shared {

std::tr1::shared_ptr<ByteBuffer> SoeMessageFactory::buildSessionResponse(
  const GalaxyServer& server, std::tr1::shared_ptr<Session> session) {
  std::tr1::shared_ptr<ByteBuffer> session_response(new ByteBuffer());

  *session_response << htons(0x0002);
  *session_response << session->connectionId();
  *session_response << htonl(session->crcSeed());
  *session_response << static_cast<uint8_t>(session->crcLength());
  *session_response << htons(server.encryptionMethod());
  *session_response << htonl(server.maxUdpSize());

  return session_response;
}


std::tr1::shared_ptr<ByteBuffer> SoeMessageFactory::buildNetworkStatusResponse(
  std::tr1::shared_ptr<Session> session, uint16_t tick) {
  std::tr1::shared_ptr<ByteBuffer> network_status_response(new ByteBuffer());

  *network_status_response << htons(0x0008);
  *network_status_response << tick;  // Client Tick Count

  for (int8_t i = 0; i < 39; ++i) {
    *network_status_response << static_cast<uint8_t>(0);
  }

  return network_status_response;
}


std::tr1::shared_ptr<ByteBuffer> SoeMessageFactory::buildKeepAliveResponse() {
  std::tr1::shared_ptr<ByteBuffer> keep_alive_response(new ByteBuffer());

  *keep_alive_response << htons(0x0006);
  *keep_alive_response << 0x00 << 0x0000;  // Footer

  return keep_alive_response;
}

}  // namespace sandbox
}  // namespace shared
