/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/


#include <exception>

#include "shared/logger.h"
#include "shared/packet_tools.h"
#include "shared/session.h"

namespace sandbox {
namespace shared {

Session::Session(const GalaxyServer& server,
  const NetworkAddress& address, Protocol<uint32_t>& protocol)
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
, received_sequence_(0) {
  // Initialize the player to a default location and state.
  // @todo: This information should be pulled from storage
  player_->position(glm::vec3(-1443, 9, 2771));
  player_->stationId(653564567);
  player_->locationName("naboo");
  player_->mood(0);
}


const GalaxyServer& Session::server() const {
  return server_;
}


std::tr1::shared_ptr<Player> Session::player() {
  return player_;
}


uint32_t Session::connectionId() const {
  return connection_id_;
}


uint32_t Session::connectionId(uint32_t id) {
  connection_id_ = id;
  return connection_id_;
}


uint32_t Session::crcLength() const {
  return crc_length_;
}


uint32_t Session::crcLength(uint32_t length) {
  crc_length_ = length;
  return crc_length_;
}


uint32_t Session::crcSeed() const {
  return crc_seed_;
}


uint32_t Session::crcSeed(uint32_t seed) {
  crc_seed_ = seed;
  return crc_seed_;
}


uint32_t Session::maxUdpSize() const {
  return max_udp_size_;
}


uint32_t Session::maxUdpSize(uint32_t size) {
  max_udp_size_ = size;
  return max_udp_size_;
}


uint16_t Session::serverSequence() const {
  return server_sequence_;
}


uint16_t Session::serverSequence(uint16_t sequence) {
  server_sequence_ = sequence;
  return server_sequence_;
}


uint16_t Session::clientSequence() const {
  return client_sequence_;
}


uint16_t Session::clientSequence(uint16_t sequence) {
  client_sequence_ = sequence;
  return client_sequence_;
}


uint16_t Session::receivedSequence() const {
  return received_sequence_;
}


uint16_t Session::receivedSequence(uint16_t sequence) {
  received_sequence_ = sequence;
  return received_sequence_;
}


void Session::queueIncomingMessage(ByteBuffer& message) {
  boost::mutex::scoped_lock lock(mutex_);
  incoming_queue_.push_back(message);
}


void Session::sendHardcodedPacket(const std::string& name, bool compressed) {
  sendHardcodedPacket(LoadPacketFromTextFile(name), compressed);
}

void Session::sendHardcodedPacket(std::unique_ptr<ByteBuffer> packet,
                                  bool compressed) {
  packet->writeAt<uint16_t>(2, static_cast<uint16_t>(htons(server_sequence_)));
  sendToRemote(std::move(packet), compressed);

  ++server_sequence_;
}

void Session::sendToRemote(std::unique_ptr<ByteBuffer> packet,
  bool compress, bool encrypt) const {
  if (compress) {
    Compress(packet.get());
  }

  if (encrypt) {
    Encrypt(packet.get(), crc_seed_);
    AppendCrc(packet.get(), crc_seed_);
  }

  server_.sendToRemote(socket_address_, std::move(packet));
}

void Session::sendHeartbeat() const {
  sendToRemote(LoadPacketFromTextFile("packets/OkPacket.txt"));
}

void Session::sendAcknowledge() const {
  std::unique_ptr<ByteBuffer> packet(
    LoadPacketFromTextFile("packets/SendAcknowledge.txt"));

  packet->writeAt<uint16_t>(2, static_cast<uint16_t>(client_sequence_));

  sendToRemote(std::move(packet));
}

void Session::sendText(const std::wstring& text, std::vector<uint64_t> moodId) {
  std::unique_ptr<ByteBuffer> message(
    LoadPacketFromTextFile("packets/Spatial/PlayerChatHeader.txt"));

  message->write<std::wstring>(text);

  message->append(*LoadPacketFromTextFile(
    "packets/Spatial/PlayerChatFooter.txt"));

  message->writeAt<uint16_t>(50 + (text.length() * 2) + 2,
    static_cast<uint16_t>(moodId[1]));

  message->writeAt<uint16_t>(50 + (text.length() * 2) + 4,
    static_cast<uint16_t>(moodId[2]));

  sendHardcodedPacket(std::move(message), true);
}

void Session::handlePacket(ByteBuffer& packet) {
  try {
    MessageHandler handler = protocol_.find(packet);
    handler(*this, packet);
  } catch(std::exception& e) {
    Logger().log(Logger::ERR) << e.what() << std::endl << packet;
    sendHeartbeat();
  }
}


void Session::update(time_t currentTime) {}

}  // namespace sandbox
}  // namespace shared
