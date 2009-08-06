/*
 * OpenSWG Combat Upgrade Sandbox - Session.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include <exception>

#include "Session.h"
#include "Logger.h"
#include "PacketTools.h"

Session::Session(const GalaxyServer& server, const NetworkAddress& address, Protocol<uint32_t>& protocol)
    : socket_address_(address)
    , server_(server)
    , player_(new Player())
    , protocol_(protocol)
    , connection_id_(0)
    , crc_length_(0)
    , crc_seed_(0xDEADBABE)
    , max_udp_size_(0)
    , server_sequence_(0)
    , client_sequence_(0)
    , received_sequence_(0)
{
    // Initialize the player to a default location and state. 
    // @todo: This information should be pulled from storage
    player_->position(Vector3<int>(-1443, 9, 2771));
    player_->stationId(653564567);
    player_->locationName("naboo");
    player_->mood(0);
}


const GalaxyServer& Session::server() const 
{
    return server_;
}


std::tr1::shared_ptr<Player> Session::player()
{
	return player_;
}


uint32_t Session::connectionId() const
{
    return connection_id_;
}


uint32_t Session::connectionId(uint32_t id)
{
    connection_id_ = id;
    return connection_id_;
}


uint32_t Session::crcLength() const
{
    return crc_length_;
}


uint32_t Session::crcLength(uint32_t length)
{
    crc_length_ = length;
    return crc_length_;
}


uint32_t Session::crcSeed() const
{
    return crc_seed_;
}


uint32_t Session::crcSeed(uint32_t seed)
{
    crc_seed_ = seed;
    return crc_seed_;
}


uint32_t Session::maxUdpSize() const
{
    return max_udp_size_;
}


uint32_t Session::maxUdpSize(uint32_t size)
{
    max_udp_size_ = size;
    return max_udp_size_;
}


uint16_t Session::serverSequence() const
{
    return server_sequence_;
}


uint16_t Session::serverSequence(uint16_t sequence)
{
    server_sequence_ = sequence;
    return server_sequence_;
}


uint16_t Session::clientSequence() const
{
    return client_sequence_;
}


uint16_t Session::clientSequence(uint16_t sequence)
{
    client_sequence_ = sequence;
    return client_sequence_;
}


uint16_t Session::receivedSequence() const
{
    return received_sequence_;
}


uint16_t Session::receivedSequence(uint16_t sequence)
{
    received_sequence_ = sequence;
    return received_sequence_;
}


void Session::queueIncomingMessage(ByteBuffer& message)
{
	boost::mutex::scoped_lock lock(mutex_);
	incoming_queue_.push_back(message);
}


void Session::sendHardcodedPacket(const std::string& name, bool compressed)
{        
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile(name);
    sendHardcodedPacket(*packet, compressed);
}

void Session::sendHardcodedPacket(ByteBuffer& packet, bool compressed)
{	
    packet.writeAt<uint16_t>(2, static_cast<uint16_t>(htons(server_sequence_)));
    sendToRemote(packet, compressed);

    ++server_sequence_;;	
}

void Session::sendToRemote(ByteBuffer& packet, bool compress, bool encrypt) const
{
    // Logger().log(INFO) << "Outgoing Packet" << std::endl << packet << std::endl;

    if (compress) {
        Compress(packet);
    }

    if (encrypt) {
        Encrypt(packet, crc_seed_);    
        AppendCrc(packet, crc_seed_);
    }

    server_.sendToRemote(socket_address_, packet);
}

void Session::sendHeartbeat() const
{
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\OkPacket.txt");

    sendToRemote(*packet);
}

void Session::sendAcknowledge() const
{
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\SendAcknowledge.txt");
    packet->writeAt<uint16_t>(2, static_cast<uint16_t>(client_sequence_));

    sendToRemote(*packet);
}

void Session::sendText(const std::wstring& text, std::vector<uint64_t> moodId)
{
    std::tr1::shared_ptr<ByteBuffer> message = LoadPacketFromTextFile("packets\\Spatial\\PlayerChatHeader.txt");
    message->write<std::wstring>(text);

    message->append(*LoadPacketFromTextFile("packets\\Spatial\\PlayerChatFooter.txt"));

    message->writeAt<uint16_t>(50 + (text.length() * 2) + 2, moodId[1]);
    message->writeAt<uint16_t>(50 + (text.length() * 2) + 4, moodId[2]);

    sendHardcodedPacket(*message, true);
}

void Session::handlePacket(ByteBuffer& packet)
{
    try {
	    MessageHandler handler = protocol_.find(packet);
        handler(*this, packet);
    } catch (std::exception& e) {
        Logger().log(ERR) << e.what() << std::endl << packet;
		sendHeartbeat();
	}
}


void Session::update(time_t currentTime)
{}

