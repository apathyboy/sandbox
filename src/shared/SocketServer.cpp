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
#include "Session.h"
#include "ByteBuffer.h"

using boost::asio::ip::udp;

class SocketServerImpl
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
  
    void sendResponse(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> buffer)
    {
        socket_.async_send_to(
            boost::asio::buffer(buffer->data(), buffer->size()), 
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
            std::tr1::shared_ptr<ByteBuffer> message(new ByteBuffer(&buffer_[0], bytes_received));
            socket_server_->handleIncoming(remote_endpoint_, message);
        } else {
            Logger().log(ERR) << error.message();
        }

  	    listen();
    }
           
    void handleSend(std::tr1::shared_ptr<ByteBuffer> message, const boost::system::error_code& error, size_t bytes_sent)
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
    pimpl_->listen();
	Logger().log(INFO) << "Server listening on port [" << pimpl_->port() << "]";

    for(;;) {
        update();

        pimpl_->poll();

        boost::this_thread::sleep(boost::posix_time::milliseconds(20));
	}
}

/** Add Session function
 *	Adds a new swg client to the client map.
 */
std::tr1::shared_ptr<Session> SocketServer::addSession(const NetworkAddress& address)
{
    Logger().log(INFO) << "Adding session for [" << address << "]";

	// Create a new session and store it in the session map. 
    std::tr1::shared_ptr<Session> session(new Session(this, address));
	sessions_[address] = session;

	// Return the new session.
	return session;
}

/** On Incoming Data function
 *	Whenever information is received via the socket this function is
 *	called to handle the data.
 */
void SocketServer::handleIncoming(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> message)
{    
	// Attempt to find the client in the session map.
	SessionMap::iterator i = sessions_.find(address);

	// If a session exists retrieve it from the session map otherwise verify
    // the message is a session request and create a new one.
    std::tr1::shared_ptr<Session> session;
	if (i != sessions_.end()) {
        session = (*i).second;
	} else if (message->peek<uint16_t>(true) == 0x0001) {        
        session = addSession(address);
	} else {
        Logger().log(ERR) << "Unexpected message from [" << address << ": " << std::endl << message;	
        return;
	}

	session->handlePacket(message);
}

/** Send Packet function
 *	Sends a packet to the specified to the specified client.
 */
void SocketServer::sendPacket(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> message) const
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

    // Check to see if enough time has passed then update the session map.
	if ((current_time_ - last_cleanup_time_) >= 1) {

        SessionMap::iterator end = sessions_.end();
        for (SessionMap::iterator i = sessions_.begin(); i !=end; ++i) {
            std::tr1::shared_ptr<Session> session = (*i).second;
			session->update(current_time_);
		}

	    last_cleanup_time_ = current_time_;
	}

    onUpdate();
}
    

time_t SocketServer::currentTime() const
{
    return current_time_;
}

