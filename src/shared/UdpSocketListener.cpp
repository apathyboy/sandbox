/*
 * OpenSWG: GalaxyEngine - UdpSocketListener.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "boost/asio.hpp"
#include "UdpSocketListener.h"
#include "ByteBuffer.h"


/** UdpSocketListener::UdpSocketListenerImpl **/ 
class UdpSocketListener::UdpSocketListenerImpl
{
public:
    UdpSocketListenerImpl(uint16_t port)
        : io_service_()
        , socket_(io_service_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
        , port_(port)
    {
  	    asyncRecieve();
    }

    uint16_t port()
    {
        return port_;
    }

    void port(uint16_t port)
    {
        socket_.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
        port_ = port;
    }

    Callback callback()
    {
        return callback_;
    }

    void callback(Callback callback)
    {
        callback_ = callback;
    }

    void poll()
    {
        io_service_.poll();
    }

    void sendToRemote(const NetworkAddress& address, ByteBuffer& message)
    {
        socket_.async_send_to(
            boost::asio::buffer(message.data(), message.size()), 
            address,
            std::tr1::bind(
                &UdpSocketListenerImpl::handleSend, 
                this, 
                message, 
                std::tr1::placeholders::_1, 
                std::tr1::placeholders::_2
            )
        );
    }
           
    void handleSend(ByteBuffer& message, const boost::system::error_code& error, size_t bytes_sent)
    {
        /* @todo: Handle send issues here */
    }

private:
    UdpSocketListenerImpl(const UdpSocketListenerImpl&);
    UdpSocketListenerImpl& operator=(const UdpSocketListenerImpl&);
  
    void handleReceive(const boost::system::error_code& error, size_t bytes_received)
    {  	
        ByteBuffer message(&buffer_[0], bytes_received);
        callback_(remote_endpoint_, message);

  	    asyncRecieve();
    }

    void asyncRecieve()
    {
  	    socket_.async_receive_from(
            boost::asio::buffer(buffer_),
  		    remote_endpoint_,
            std::tr1::bind(
                &UdpSocketListenerImpl::handleReceive, 
                this, 
                std::tr1::placeholders::_1, 
                std::tr1::placeholders::_2
            )
        );  
    }

    boost::asio::io_service         io_service_;
    boost::asio::ip::udp::socket    socket_;
    boost::asio::ip::udp::endpoint  remote_endpoint_;
    
    UdpSocketListener::Callback     callback_;

    std::vector<uint8_t>            buffer_;

    uint16_t port_;
};


/** UdpSocketListener **/

UdpSocketListener::UdpSocketListener(uint16_t port)
    : impl_(new UdpSocketListener::UdpSocketListenerImpl(port))
{}


UdpSocketListener::~UdpSocketListener()
{}
    

uint16_t UdpSocketListener::port()
{
    return impl_->port();
}


void UdpSocketListener::port(uint16_t port)
{
    impl_->port(port);
}


UdpSocketListener::Callback UdpSocketListener::callback()
{
    return impl_->callback();
}
    
void UdpSocketListener::callback(UdpSocketListener::Callback callback)
{
    impl_->callback(callback);
}


void UdpSocketListener::poll()
{
    impl_->poll();
}


void UdpSocketListener::sendToRemote(const NetworkAddress &address, ByteBuffer& message) const
{
    impl_->sendToRemote(address, message);
}

