/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SRC_SHARED_SOE_MESSAGE_FACTORY_H_
#define SRC_SHARED_SOE_MESSAGE_FACTORY_H_

#include <cstdint>

#ifdef _MSC_VER
#include <memory>
#else
#include <tr1/memory>  // NOLINT
#endif

namespace sandbox {
namespace shared {

class ByteBuffer;
class Session;
class GalaxyServer;

class SoeMessageFactory {
 public:
  static std::tr1::shared_ptr<ByteBuffer> buildSessionResponse(
    const GalaxyServer& server, std::tr1::shared_ptr<Session> session);

  static std::tr1::shared_ptr<ByteBuffer> buildNetworkStatusResponse(
    std::tr1::shared_ptr<Session> session, uint16_t tick);

  static std::tr1::shared_ptr<ByteBuffer> buildKeepAliveResponse();

 private:
  SoeMessageFactory();
  ~SoeMessageFactory();

  SoeMessageFactory(const SoeMessageFactory&);
  SoeMessageFactory& operator=(const SoeMessageFactory&);
};

}  // namespace sandbox
}  // namespace shared

#endif  // SRC_SHARED_SOE_MESSAGE_FACTORY_H_
