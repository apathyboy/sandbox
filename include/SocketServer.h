/*
 * OpenSWG Combat Upgrade Sandbox - SocketServer.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef OPENSWG_SOCKET_SERVER_H
#define OPENSWG_SOCKET_SERVER_H

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <map>
#include <tr1/memory>

#include "ByteBuffer.h"
#include "NetworkAddress.h"
#include "Protocol.h"

/** SocketServer class
 *  This class handles the sending and receiving of data from the client.
 */
class SocketServer
{
public:
    explicit SocketServer(uint16_t port);
    virtual ~SocketServer();

	void run();

    void handleIncoming(const NetworkAddress& address, ByteBuffer& message);

	void update();

    void sendPacket(const NetworkAddress& address, ByteBuffer& message) const;

    time_t currentTime() const;

	uint16_t port() const;

private:	
	SocketServer();
    SocketServer(const SocketServer&);
    SocketServer& operator=(const SocketServer&);

    virtual void onIncoming(const NetworkAddress& address, ByteBuffer& message) = 0;
	virtual void onUpdate() = 0;
    virtual void initializeProtocol() = 0;

    class SocketServerImpl;
    std::tr1::shared_ptr<SocketServerImpl> pimpl_;

	time_t current_time_;
    time_t last_cleanup_time_;
};

#endif // OPENSWG_SOCKET_SERVER_H