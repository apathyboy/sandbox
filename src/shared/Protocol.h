/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SRC_SHARED_PROTOCOL_H_
#define SRC_SHARED_PROTOCOL_H_

#include <cstdint>

#ifdef _MSC_VER
#include <functional>
#include <memory>
#else
#include <tr1/functional>  // NOLINT
#include <tr1/memory>      // NOLINT
#endif

#include <map>

#include "shared/byte_buffer.h"

namespace sandbox {
namespace shared {

class Session;

typedef std::tr1::function<void (Session& session, ByteBuffer&)> MessageHandler;

template<typename Identifier, class Handler = MessageHandler>

class Protocol {
 public:
  Protocol() {}
  ~Protocol() {}

  void addHandler(Identifier id, Handler handler) {
    handlers_.insert(std::make_pair(id, handler));
  }

  Handler find(Identifier id) {
    Handler handler = NULL;

    typename std::map<Identifier, Handler>::iterator i = handlers_.find(id);
    if (i != handlers_.end()) {
      handler = (*i).second;
    }

    return handler;
  }

  Handler find(ByteBuffer& message) {
    Handler handler = NULL;

    typename std::map<Identifier, Handler>::iterator i =
      handlers_.find(message.peekAt<uint8_t>(1));

    if (i != handlers_.end()) {
      handler = (*i).second;
    }

    if (!handler) {
      i = handlers_.find(message.peekAt<uint32_t>(2));

      if (i != handlers_.end()) {
        handler = (*i).second;
      } else {
        throw std::exception();
      }
    }

    return handler;
  }

 private:
  Protocol(const Protocol&);
  Protocol& operator=(const Protocol&);

  std::map<Identifier, Handler> handlers_;
};

}  // namespace sandbox
}  // namespace shared


#endif  // SRC_SHARED_PROTOCOL_H_
