/**
 * *********************************************************************
 * OpenSWG Sandbox Server
 * Copyright (C) 2006 OpenSWG Team <http://www.openswg.com>
 * *********************************************************************
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * To read the license please visit http://www.gnu.org/copyleft/gpl.html
 * *********************************************************************
 */

#include <string>
#include <vector>
#include <tr1/memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "SocketServer.h"
#include "Logger.h"
#include "GalaxySession.h"
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


    const uint16_t port()
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
        std::tr1::shared_ptr<ByteBuffer> message(new ByteBuffer(&buffer_[0], bytes_received));
        socket_server_->handleIncoming(remote_endpoint_, message);

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

    for(;;)
    {
        update();

        pimpl_->poll();

        boost::this_thread::yield();
        boost::this_thread::interruption_point();
	}
}

/** Add GalaxySession function
 *	Adds a new swg client to the client map.
 */
std::tr1::shared_ptr<GalaxySession> SocketServer::addGalaxySession(const NetworkAddress& address)
{
    std::string ip = address.address().to_string();

    Logger().log(INFO) << "Adding session for [" << address << "]";

	// Create a new session and store it in the session map. 
    std::tr1::shared_ptr<GalaxySession> session(new GalaxySession(this, address, ip));
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
	GalaxySessionMap::iterator i = sessions_.find(address);

	// If the session exists retrieve it from the session map and send the
	// packet data on to it's packet handler.
	if (i != sessions_.end())
	{
        std::tr1::shared_ptr<GalaxySession> session = (*i).second;
		session->PrepPacket(message);	

		session->HandlePacket(message);
	}

	// Otherwise create a new client which stores it in the client map
	// and then send the new client the packet data.
	else
	{
        if (message->peek<uint16_t>(true) == 0x0001) 
		{        
            std::tr1::shared_ptr<GalaxySession> session = addGalaxySession(address);
			session->PrepPacket(message);

			session->HandlePacket(message);	
		}
		else
		{
			Logger().log(ERR) << "Unexpected Opcode [" << message->peek<uint16_t>() << "] from invalid client [" << address << "]";
		}
	}
}

/** Send Packet function
 *	Sends a packet to the specified to the specified client.
 */
void SocketServer::sendPacket(const NetworkAddress& address, char *packet, unsigned short length)
{	
    std::tr1::shared_ptr<ByteBuffer> buffer(new ByteBuffer(reinterpret_cast<unsigned char *>(packet), length));
    
    pimpl_->sendResponse(address, buffer);
}

const uint16_t SocketServer::port()
{
    return pimpl_->port();
}

void SocketServer::update()
{
    current_time_ = time(0);

    // Check to see if enough time has passed then update the session map.
	if ((current_time_ - last_cleanup_time_) >= 1) {

        GalaxySessionMap::iterator end = sessions_.end();
		for (GalaxySessionMap::iterator i = sessions_.begin(); i !=end; ++i)
		{
            std::tr1::shared_ptr<GalaxySession> session = (*i).second;
			session->Update(current_time_);
		}

	    last_cleanup_time_ = current_time_;
	}

    onUpdate();
}