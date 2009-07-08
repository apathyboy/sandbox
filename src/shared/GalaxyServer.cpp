/*
 * OpenSWG: GalaxyEngine - GalaxyServer.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "boost/thread.hpp"
#include "GalaxyServer.h"
#include "Logger.h"
#include "Session.h"
#include "SoeMessageFactory.h"

GalaxyServer::GalaxyServer(uint16_t port)
    : network_listener_(port)
{    
    soe_protocol_.addHandler(0x0001, std::tr1::bind(&GalaxyServer::handleSessionRequest, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(0x0003, std::tr1::bind(&GalaxyServer::handleMultiPacket, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(0x0005, std::tr1::bind(&GalaxyServer::handleDisconnect, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(0x0006, std::tr1::bind(&GalaxyServer::handlePing, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(0x0007, std::tr1::bind(&GalaxyServer::handleNetStatus, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(0x0009, std::tr1::bind(&GalaxyServer::handleDataChannel, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(0x0015, std::tr1::bind(&GalaxyServer::handleAcknowledge, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}


GalaxyServer::~GalaxyServer()
{}


uint16_t GalaxyServer::port()
{
    return network_listener_.port();
}


void GalaxyServer::port(uint16_t port)
{
    network_listener_.port(port);
}


void GalaxyServer::run()
{
    initializeProtocol();
        
    for(;;) {
        onUpdate();

        network_listener_.poll();

        boost::this_thread::sleep(boost::posix_time::milliseconds(20));
	}
}


void GalaxyServer::handleIncoming(const NetworkAddress& address, ByteBuffer& message)
{
    uint16_t opcode = message.read<uint16_t>();
    
	SoeMessageHandler handler = soe_protocol_.find(opcode);

    if (! handler) {
        Logger().log(INFO) << "Unidentified message received" << std::endl << message;
        return;
    }

    handler(address, message);
}


void GalaxyServer::sendToRemote(const NetworkAddress& address, ByteBuffer& message) const
{
    network_listener_.sendToRemote(address, message);
}


std::tr1::shared_ptr<Session> GalaxyServer::addSession(const NetworkAddress& address)
{
    Logger().log(INFO) << "Adding session for [" << address << "]";

	// Create a new session and store it in the session map. 
    std::tr1::shared_ptr<Session> session(new Session(*this, address, swg_protocol_));
	sessions_[address] = session;

	// Return the new session.
	return session;
}


std::tr1::shared_ptr<Session> GalaxyServer::findSession(const NetworkAddress& address)
{
    std::tr1::shared_ptr<Session> session;

	SessionMap::iterator i = sessions_.find(address);
	if (i != sessions_.end()) {
        session = (*i).second;
	}

    return session;
}


std::tr1::shared_ptr<Session> GalaxyServer::removeSession(const NetworkAddress& address)
{
    std::tr1::shared_ptr<Session> session;

    SessionMap::iterator i = sessions_.find(address);
    if (i != sessions_.end()) {
        session = (*i).second;
        sessions_.erase(i);
    }

    return session;
}


uint32_t GalaxyServer::sessionCount() const
{
    return sessions_.size();
}


void GalaxyServer::handleSessionRequest(const NetworkAddress& address, ByteBuffer& message)
{
    std::tr1::shared_ptr<Session> session = findSession(address);

    if (session) {
        Logger().log(ERR) << "Received session request from an address with an existing session: [" << address << "]";
        return;
    }

    session = addSession(address);

    session->crcLength(message.read<uint32_t>());
    session->connectionId(message.read<uint32_t>());
    session->maxUdpSize(message.read<uint32_t>());

    std::tr1::shared_ptr<ByteBuffer> session_response(SoeMessageFactory::buildSessionResponse(session));
    sendToRemote(address, *session_response);
}


void GalaxyServer::handleNetStatus(const NetworkAddress& address, ByteBuffer& message)
{
    std::tr1::shared_ptr<Session> session = findSession(address);

    if (! session) {
        Logger().log(ERR) << "Received a Network Status message from an address without a sesion: [" << address << "]";
        return;
    }

    std::tr1::shared_ptr<ByteBuffer> session_response(SoeMessageFactory::buildNetworkStatusResponse(session));
    sendToRemote(address, *session_response);
}


void GalaxyServer::handleMultiPacket(const NetworkAddress& address, ByteBuffer& message)
{
    // Loop through the message until the compression bit is reached.
    while (message.readPosition() < message.size() - 4)
    {
        uint8_t segment_size = message.read<uint8_t>();
        
        // If the segment size is 255+ check the next bit, a 0x01 indicates
        // over 255 and that the next bit should be added to the total
        // size. If 0x00 then the next bit should be skipped.
        if (segment_size == 0xFF) {
            if (message.read<uint8_t>() == 0x01) {
                segment_size += message.read<uint8_t>();
            } else {
                message.read<uint8_t>();
            }
        }
    
        ByteBuffer segment(message.data()+message.readPosition(), segment_size);
        message.readPosition(message.readPosition() + segment_size);

		handleIncoming(address, segment);
    }
}


void GalaxyServer::handleAcknowledge(const NetworkAddress& address, ByteBuffer& message)
{
    std::tr1::shared_ptr<Session> session = findSession(address);

    if (! session) {
        Logger().log(ERR) << "Received an Acknowledge message from an address without a sesion: [" << address << "]";
        return;
    }

    session->receivedSequence(message.read<uint16_t>());
    session->clientSequence(message.read<uint16_t>());
}


void GalaxyServer::handleDataChannel(const NetworkAddress& address, ByteBuffer& message)
{}


void GalaxyServer::handleDisconnect(const NetworkAddress& address, ByteBuffer& message)
{}


void GalaxyServer::handlePing(const NetworkAddress& address, ByteBuffer& message)
{}

