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

/**	Galaxy Session constructor
 *	Takes the data necessary for the Session class to function.
 */
Session::Session(const SocketServer * const server, const NetworkAddress& address, Protocol<uint32_t>& protocol)
    : socket_address_(address)
    , socket_server_(server)
    , player_(new Player())
    , protocol_(protocol)
    , server_sequence_(0)
    , client_sequence_(0)
    , received_sequence_(0)
    , connection_id_(0)
    , crc_seed_(0xDEADBABE)
{
    // Initialize the player to a default location and state. 
    // @todo: This information should be pulled from storage
    player_->position(Vector3<int>(-1443, 9, 2771));
    player_->stationId(653564567);
    player_->locationName("naboo");
    player_->mood(0);
}


const SocketServer * const Session::server() const
{
    return socket_server_;
}


std::tr1::shared_ptr<Player> Session::player()
{
	return player_;
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


uint32_t Session::connectionId() const
{
    return connection_id_;
}


uint32_t Session::connectionId(uint32_t id)
{
    connection_id_ = id;
    return connection_id_;
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

void Session::sendHardcodedPacket(const std::string& name, bool compressed)
{        
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile(name);
    sendHardcodedPacket(packet, compressed);
}

void Session::sendHardcodedPacket(std::tr1::shared_ptr<ByteBuffer> packet, bool compressed)
{	
    packet->writeAt<uint16_t>(2, static_cast<uint16_t>(htons(server_sequence_)));
    sendToRemote(packet, true, compressed, true);

    ++server_sequence_;;	
}

void Session::sendToRemote(std::tr1::shared_ptr<ByteBuffer> packet, bool encrypt, bool compress, bool crc) const
{
    //Logger().log(INFO) << "Outgoing Packet" << std::endl << *packet << std::endl;

    if (compress) {
        Compress(packet);
    }

    if (encrypt) {
        Encrypt(packet, crc_seed_);    
    }

    if (crc) {
        AppendCrc(packet, crc_seed_);
    }

    socket_server_->sendPacket(socket_address_, packet);
}

void Session::sendHeartbeat() const
{
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\OkPacket.txt");

    sendToRemote(packet, true);
}

void Session::sendAcknowledge() const
{
    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\SendAcknowledge.txt");
    packet->writeAt<uint16_t>(2, static_cast<uint16_t>(client_sequence_));

    sendToRemote(packet, true);
}

void Session::sendText(const std::wstring& text, std::vector<uint64_t> moodId)
{
    std::tr1::shared_ptr<ByteBuffer> message = LoadPacketFromTextFile("packets\\Spatial\\PlayerChatHeader.txt");
    message->write<std::wstring>(text);

    message->append(*LoadPacketFromTextFile("packets\\Spatial\\PlayerChatFooter.txt"));

    message->writeAt<uint16_t>(50 + (text.length() * 2) + 2, moodId[1]);
    message->writeAt<uint16_t>(50 + (text.length() * 2) + 4, moodId[2]);

    sendHardcodedPacket(message, true);
}

void Session::handlePacket(std::tr1::shared_ptr<ByteBuffer> packet)
{
    // Decrypt and decompress the incoming data as needed.
    if(CrcTest(packet, crc_seed_)) {
        Decrypt(packet, crc_seed_);
    }

    if (packet->peekAt<uint8_t>(2) == 'x') {
        Decompress(packet);
    }

    Logger().log(INFO) << "Incoming Message" << std::endl << *packet;

    try {
	    MessageHandler handler = protocol_.find(packet);
        handler(*this, packet);
    } catch (std::exception& e) {
        Logger().log(ERR) << e.what() << std::endl << *packet;
		sendHeartbeat();
	}
}


void Session::update(time_t currentTime)
{}

