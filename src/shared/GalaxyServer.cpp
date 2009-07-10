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
#include "PacketTools.h"

GalaxyServer::GalaxyServer(uint16_t port)
    : network_listener_(port)
    , encryption_method_(0x0104)
    , max_udp_size_(496)
{    
    soe_protocol_.addHandler(htons(0x0001), std::tr1::bind(&GalaxyServer::handleSessionRequest, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(htons(0x0003), std::tr1::bind(&GalaxyServer::handleMultiMessage,   this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(htons(0x0005), std::tr1::bind(&GalaxyServer::handleDisconnect,     this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(htons(0x0006), std::tr1::bind(&GalaxyServer::handleKeepAlive,      this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(htons(0x0007), std::tr1::bind(&GalaxyServer::handleNetStatus,      this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(htons(0x0009), std::tr1::bind(&GalaxyServer::handleDataChannel,    this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    soe_protocol_.addHandler(htons(0x0015), std::tr1::bind(&GalaxyServer::handleAcknowledge,    this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}


GalaxyServer::~GalaxyServer()
{}


uint16_t GalaxyServer::port() const
{
    return network_listener_.port();
}


void GalaxyServer::port(uint16_t port)
{
    network_listener_.port(port);
}

    
int16_t GalaxyServer::encryptionMethod() const
{
    return encryption_method_;
}


void GalaxyServer::encryptionMethod(int16_t encryptionMethod)
{
    encryption_method_ = encryptionMethod;
}


uint32_t GalaxyServer::maxUdpSize() const
{
    return max_udp_size_;
}


void GalaxyServer::maxUdpSize(uint32_t size)
{
    max_udp_size_ = size;
}


void GalaxyServer::run()
{
    initializeProtocol();
    
    network_listener_.callback(std::tr1::bind(&GalaxyServer::handleIncoming, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
        
    for(;;) {
        onUpdate();

        network_listener_.poll();

        boost::this_thread::sleep(boost::posix_time::milliseconds(20));
	}
}


void GalaxyServer::handleIncoming(const NetworkAddress& address, ByteBuffer& message)
{
    std::tr1::shared_ptr<Session> session = findSession(address);

   // Logger().log(INFO) << "Unprocessed Incoming Message" << std::endl << message;

    if (session) {    
        if(CrcTest(message, session->crcSeed(), session->crcLength())) {
            Decrypt(message, session->crcSeed(), session->crcLength());
        }

        if (message.peekAt<uint8_t>(2) == 'x') {
            Decompress(message);
        }
    }

    // Logger().log(INFO) << "Incoming Message" << std::endl << message;

    handleMessage(address, message);
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
    

void GalaxyServer::addSwgProtocolHandler(uint32_t identifier, MessageHandler handler)
{
    swg_protocol_.addHandler(identifier, handler);   
}


void GalaxyServer::handleMessage(const NetworkAddress& address, ByteBuffer& message)
{
    uint16_t opcode = message.read<uint16_t>();
    
	SoeMessageHandler handler = soe_protocol_.find(opcode);

    if (! handler) {
        Logger().log(INFO) << "Unidentified message received" << std::endl << message;
        return;
    }

    handler(address, message);
}


void GalaxyServer::handleSessionRequest(const NetworkAddress& address, ByteBuffer& message)
{
    std::tr1::shared_ptr<Session> session = findSession(address);

    if (session) {
        Logger().log(ERR) << "Received session request from an address with an existing session: [" << address << "]";
        return;
    }

    session = addSession(address);

    session->crcLength(ntohl(message.read<uint32_t>()));
    session->connectionId(message.read<uint32_t>());
    session->maxUdpSize(ntohl(message.read<uint32_t>()));

    std::tr1::shared_ptr<ByteBuffer> session_response(SoeMessageFactory::buildSessionResponse(*this, session));
    session->sendToRemote(*session_response, false, false);
}


void GalaxyServer::handleNetStatus(const NetworkAddress& address, ByteBuffer& message)
{
    std::tr1::shared_ptr<Session> session = findSession(address);

    if (! session) {
        Logger().log(ERR) << "Received a Network Status message from an address without a session: [" << address << "]";
        return;
    }
    
    uint16_t tick = message.read<uint16_t>();

    std::tr1::shared_ptr<ByteBuffer> network_status_response(SoeMessageFactory::buildNetworkStatusResponse(session, tick));
    session->sendToRemote(*network_status_response, true, false);
}


void GalaxyServer::handleMultiMessage(const NetworkAddress& address, ByteBuffer& message)
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

		handleMessage(address, segment);
    }
}


void GalaxyServer::handleAcknowledge(const NetworkAddress& address, ByteBuffer& message)
{
    std::tr1::shared_ptr<Session> session = findSession(address);

    if (! session) {
        Logger().log(ERR) << "Received an Acknowledge message from an address without a session: [" << address << "]";
        return;
    }

    // @TODO: Complete this implementation correctly.
    uint16_t sequence = message.read<uint16_t>();
    
    session->receivedSequence(sequence);
    session->clientSequence(sequence);
}


void GalaxyServer::handleDataChannel(const NetworkAddress& address, ByteBuffer& message)
{
    std::tr1::shared_ptr<Session> session = findSession(address);

    if (! session) {
        Logger().log(ERR) << "Received a Data Channel message from an address without a session: [" << address << "]";
        return;
    }

    session->clientSequence(message.read<uint16_t>());
    session->sendAcknowledge();

    if (ntohs(message.peek<uint16_t>()) == 0x0019) {
        message.read<uint16_t>();

        // Loop through the message until the compression bit is reached.
        while (message.readPosition() < message.size() - 3)
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
		    session->handlePacket(segment);

            message.readPosition(message.readPosition() + segment_size);
        }        
    } else {
    
        ByteBuffer segment(message.data()+message.readPosition(), message.size()-7);
		session->handlePacket(segment);
    }
}


void GalaxyServer::handleDisconnect(const NetworkAddress& address, ByteBuffer& message)
{
    std::tr1::shared_ptr<Session> session = findSession(address);

    if (! session) {
        Logger().log(ERR) << "Received a Disconnect message from an address without a session: [" << address << "]";
        return;
    }

    removeSession(address);
}


void GalaxyServer::handleKeepAlive(const NetworkAddress& address, ByteBuffer& message)
{
    std::tr1::shared_ptr<Session> session = findSession(address);

    if (! session) {
        Logger().log(ERR) << "Received a Keep Alive message from an address without a session: [" << address << "]";
        return;
    }

    std::tr1::shared_ptr<ByteBuffer> keep_alive_response(SoeMessageFactory::buildKeepAliveResponse());
    session->sendToRemote(*keep_alive_response);
}

