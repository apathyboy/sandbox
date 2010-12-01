/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include "shared/udp_socket_listener.h"

#include <boost/asio.hpp>

#include "shared/byte_buffer.h"
#include "shared/logger.h"

namespace sandbox {
namespace shared {

/** UdpSocketListener::UdpSocketListenerImpl **/
class UdpSocketListener::UdpSocketListenerImpl {
 public:
  explicit UdpSocketListenerImpl(uint16_t port)
  : io_service_()
  , socket_(io_service_,
            boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
  , buffer_(496)
  , port_(port) {
    asyncRecieve();
  }

  uint16_t port() const {
    return port_;
  }

  void port(uint16_t port) {
    socket_.bind(boost::asio::ip::udp::endpoint(
      boost::asio::ip::udp::v4(), port));
    port_ = port;
  }

  UdpSocketListener::Callback callback() {
    return callback_;
  }

  void callback(UdpSocketListener::Callback callback) {
    callback_ = callback;
  }

  void poll() {
    io_service_.poll();
  }

  void sendToRemote(const NetworkAddress& address,
                    std::unique_ptr<ByteBuffer> message) {
    socket_.async_send_to(boost::asio::buffer(message->data(), message->size()),
      address,
      std::tr1::bind(
        &UdpSocketListenerImpl::handleSend, this,
        std::tr1::placeholders::_1,
        std::tr1::placeholders::_2));
  }

  void handleSend(const boost::system::error_code& error, size_t bytes_sent) {
      /* @todo: Handle send issues here */
      error, bytes_sent;
  }

 private:
  UdpSocketListenerImpl(const UdpSocketListenerImpl&);
  UdpSocketListenerImpl& operator=(const UdpSocketListenerImpl&);

  void handleReceive(const boost::system::error_code& error,
                     size_t bytes_received) {
    if (error && error != boost::asio::error::message_size) {
      return;
    }

    ByteBuffer message(&buffer_[0], bytes_received);
    callback_(remote_endpoint_, message);

    asyncRecieve();
  }

  void asyncRecieve() {
    socket_.async_receive_from(boost::asio::buffer(buffer_),
      remote_endpoint_,
        std::tr1::bind(
          &UdpSocketListenerImpl::handleReceive,
          this,
          std::tr1::placeholders::_1,
          std::tr1::placeholders::_2));
  }

  boost::asio::io_service io_service_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint remote_endpoint_;

  UdpSocketListener::Callback callback_;

  std::vector<uint8_t> buffer_;

  uint16_t port_;
};


/** UdpSocketListener **/

UdpSocketListener::UdpSocketListener(uint16_t port)
: impl_(new UdpSocketListener::UdpSocketListenerImpl(port)) {}

UdpSocketListener::~UdpSocketListener() {}

uint16_t UdpSocketListener::port() const {
  return impl_->port();
}


void UdpSocketListener::port(uint16_t port) {
  impl_->port(port);
}

UdpSocketListener::Callback UdpSocketListener::callback() {
  return impl_->callback();
}

void UdpSocketListener::callback(UdpSocketListener::Callback callback) {
  impl_->callback(callback);
}

void UdpSocketListener::poll() {
  impl_->poll();
}

void UdpSocketListener::sendToRemote(const NetworkAddress &address,
  std::unique_ptr<ByteBuffer> message) const {
  impl_->sendToRemote(address, std::move(message));
}

}  // namespace sandbox
}  // namespace shared
