/**
 * SWG:ANH Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
 */

#include "login/login_server.h"
#include "shared/logger.h"
#include "shared/login_handler.h"
#include "shared/session.h"

namespace sandbox {
namespace login {

LoginServer::LoginServer(uint16_t port)
: shared::GalaxyServer(port) {
  shared::Logger().log(shared::INFO) <<
    "Login Server listening on port: [" << port << "]";
}


LoginServer::~LoginServer() {}

void LoginServer::onIncoming(const shared::NetworkAddress& address,
                             shared::ByteBuffer& message) {}


void LoginServer::onUpdate() {}

void LoginServer::initializeProtocol() {
    addSwgProtocolHandler(0x41131F96,
      std::tr1::bind(&shared::HandleAuthentication,
                     std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    addSwgProtocolHandler(0xD5899226,
      std::tr1::bind(&shared::HandleSession,
                     std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}

}  // namespace login
}  // namespace sandbox
