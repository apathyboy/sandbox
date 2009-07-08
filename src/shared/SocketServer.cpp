/*
 * OpenSWG Combat Upgrade Sandbox - SocketServer.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include <string>
#include <vector>
#include <tr1/memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "SocketServer.h"
#include "Logger.h"
#include "ByteBuffer.h"

using boost::asio::ip::udp;

class SocketServer::SocketServerImpl
{
public:
    SocketServerImpl(SocketServer* server, uint16_t port)
        : socket_server_(server)
        , io_service_()
        , socket_(io_service_, udp::endpoint(udp::v4(), port))
        , buffer_(max_length) // Consider setting this from input (preferably from configuration?)
        , port_(port)
    {}


    uint16_t port() const
    {
        return port_;
    }
    
    void listen() 
    {
  	    socket_.async_receive_from(
            boost::asio::buffer(buffer_),
  		    remote_endpoint_,
            std::tr1::bind(
                &SocketServerImpl::handleReceive, 
                this, 
                std::tr1::placeholders::_1, 
                std::tr1::placeholders::_2
            )
        );    
    }

    void poll()
    {
        io_service_.poll();
    }
  
    void sendResponse(const NetworkAddress& address, ByteBuffer& buffer)
    {
        socket_.async_send_to(
            boost::asio::buffer(buffer.data(), buffer.size()), 
            address,
            std::tr1::bind(
                &SocketServerImpl::handleSend, 
                this, 
                buffer, 
                std::tr1::placeholders::_1, 
                std::tr1::placeholders::_2
            )
        );
    }
           
    void handleReceive(const boost::system::error_code& error, size_t bytes_received)
    {  	
        if (! error && bytes_received > 0) {
            ByteBuffer message(&buffer_[0], bytes_received);
            socket_server_->handleIncoming(remote_endpoint_, message);
        } else {
            Logger().log(ERR) << error.message();
        }

  	    listen();
    }
           
    void handleSend(ByteBuffer& message, const boost::system::error_code& error, size_t bytes_sent)
    {}

private:

    SocketServer* socket_server_;

	boost::asio::io_service 		io_service_;
	boost::asio::ip::udp::socket 	socket_;
	boost::asio::ip::udp::endpoint 	remote_endpoint_;

    enum { max_length = 496 };
    std::vector<unsigned char> buffer_;
  
    uint16_t port_;    
};


/** Socket Server constructor
 *	Initializes the Socket Servers data members.
 */
SocketServer::SocketServer(uint16_t port)
    : pimpl_(new SocketServerImpl(this, port))
{}

SocketServer::~SocketServer()
{}

/** Run Server function
 *	Begins listening on the port specified in the configuration until
 *	the server status is no longer set to running.
 */
void SocketServer::run()
{	
    initializeProtocol();

    pimpl_->listen();
	Logger().log(INFO) << "Server listening on port [" << pimpl_->port() << "]";

    for(;;) {
        update();

        pimpl_->poll();

        boost::this_thread::sleep(boost::posix_time::milliseconds(20));
	}
}

/** On Incoming Data function
 *	Whenever information is received via the socket this function is
 *	called to handle the data.
 */
void SocketServer::handleIncoming(const NetworkAddress& address, ByteBuffer& message)
{    
    onIncoming(address, message);
}

/** Send Packet function
 *	Sends a packet to the specified to the specified client.
 */
void SocketServer::sendPacket(const NetworkAddress& address, ByteBuffer& message) const
{	
    pimpl_->sendResponse(address, message);
}

uint16_t SocketServer::port() const
{
    return pimpl_->port();
}

void SocketServer::update()
{
    current_time_ = time(0);

    onUpdate();
}
    

time_t SocketServer::currentTime() const
{
    return current_time_;
}

