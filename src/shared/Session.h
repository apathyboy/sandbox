/*
 * OpenSWG Combat Upgrade Sandbox - Session.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef SESSION_H_
#define SESSION_H_

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <queue>
#include <tr1/memory>

#include <boost/thread.hpp>

#include "ByteBuffer.h"
#include "NetworkAddress.h"
#include "Player.h"
#include "GalaxyServer.h"
#include "Protocol.h"

/** Galaxy Session class
 *	Manages the client -> server and server <- client session.
 */
class Session
{
public:
	/**	Galaxy Session constructor
	 *	Takes the data necessary for the Session class to function.
	 */
    Session(const GalaxyServer& server, const NetworkAddress& address, Protocol<uint32_t>& protocol);

    const GalaxyServer& server() const;
    std::tr1::shared_ptr<Player> player();

    uint32_t connectionId() const;
    uint32_t connectionId(uint32_t id);

    uint32_t crcLength() const;
    uint32_t crcLength(uint32_t length);

    uint32_t crcSeed() const;
    uint32_t crcSeed(uint32_t seed);

    uint32_t maxUdpSize() const;
    uint32_t maxUdpSize(uint32_t size);

    uint16_t serverSequence() const;
    uint16_t serverSequence(uint16_t sequence);

    uint16_t clientSequence() const;
    uint16_t clientSequence(uint16_t sequence);

    uint16_t receivedSequence() const;
    uint16_t receivedSequence(uint16_t sequence);

	void queueIncomingMessage(ByteBuffer& message);
    
    void sendHardcodedPacket(const std::string& name, bool compressed);
    void sendHardcodedPacket(ByteBuffer& packet, bool compressed);

    void sendToRemote(ByteBuffer& packet, bool compress = false, bool encrypt = true) const;

    void sendHeartbeat() const;
    void sendAcknowledge() const;
    void sendText(const std::wstring& text, std::vector<uint64_t> moodId);

	void handlePacket(ByteBuffer& packet);

	void update(time_t currentTime);

private:
    /* Disable unused compiler generated methods */
    Session();
    Session(const Session&);
    Session& operator=(const Session&);    

	NetworkAddress socket_address_;
    const GalaxyServer& server_;
    std::tr1::shared_ptr<Player> player_;
    Protocol<uint32_t>& protocol_;

	std::deque<ByteBuffer> incoming_queue_;

	boost::mutex mutex_;

	uint32_t connection_id_;
    uint32_t crc_length_;
    uint32_t crc_seed_;
    uint32_t max_udp_size_;
	uint16_t server_sequence_;
	uint16_t client_sequence_;
	uint16_t received_sequence_;
};

#endif // SESSION_H_