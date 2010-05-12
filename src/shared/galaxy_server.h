/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SRC_SHARED_GALAXY_SERVER_H_
#define SRC_SHARED_GALAXY_SERVER_H_

#include <cstdint>
#include <map>

#include "shared/network_address.h"
#include "shared/protocol.h"
#include "shared/udp_socket_listener.h"

namespace sandbox {
namespace shared {

class Session;

class GalaxyServer {
 public:
  explicit GalaxyServer(uint16_t port);
  virtual ~GalaxyServer();

  uint16_t port() const;
  void port(uint16_t);

  int16_t encryptionMethod() const;
  void encryptionMethod(int16_t encryptionType);

  uint32_t maxUdpSize() const;
  void maxUdpSize(uint32_t size);

  void run();

  void handleIncoming(const NetworkAddress& address, ByteBuffer& message);
  void sendToRemote(const NetworkAddress& address, ByteBuffer& message) const;

  std::tr1::shared_ptr<Session> addSession(const NetworkAddress& address);
  std::tr1::shared_ptr<Session> findSession(const NetworkAddress& address);
  std::tr1::shared_ptr<Session> removeSession(const NetworkAddress& address);
  uint32_t sessionCount() const;

  void addSwgProtocolHandler(uint32_t identifier, MessageHandler handler);

 private:
  /* Disable compiler generated methods */
  GalaxyServer();

  GalaxyServer(const GalaxyServer&);
  GalaxyServer& operator=(const GalaxyServer&);

  bool handleMessage(const NetworkAddress& address, ByteBuffer& message);

  /* SOE level handlers */
  void handleSessionRequest(const NetworkAddress& address, ByteBuffer& message);
  void handleNetStatus(const NetworkAddress& address, ByteBuffer& message);
  void handleMultiMessage(const NetworkAddress& address, ByteBuffer& message);
  void handleAcknowledge(const NetworkAddress& address, ByteBuffer& message);
  void handleDataChannel(const NetworkAddress& address, ByteBuffer& message);
  void handleDisconnect(const NetworkAddress& address, ByteBuffer& message);
  void handleKeepAlive(const NetworkAddress& address, ByteBuffer& message);

  virtual void initializeProtocol() = 0;
  virtual void onIncoming(const NetworkAddress& address,
                          ByteBuffer& message) = 0;
  virtual void onUpdate() = 0;

  typedef std::map<NetworkAddress, std::tr1::shared_ptr<Session>> SessionMap;
  SessionMap sessions_;

  UdpSocketListener network_listener_;

  typedef std::tr1::function<void (const NetworkAddress& address, ByteBuffer&)>
    SoeMessageHandler;

  Protocol<uint16_t, SoeMessageHandler> soe_protocol_;
  Protocol<uint32_t> swg_protocol_;

  uint32_t max_udp_size_;
  int16_t  encryption_method_;
};

}  // namespace sandbox
}  // namespace shared

#endif  // SRC_SHARED_GALAXY_SERVER_H_
