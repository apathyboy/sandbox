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

class Session;
class SocketServerImpl;

/** SocketServer class
 *  This class handles the sending and receiving of data from the client.
 */
class SocketServer
{
public:
    explicit SocketServer(uint16_t port);
    virtual ~SocketServer();

	void run();

    void handleIncoming(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> message);

	void update();

    void sendPacket(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> message) const;

    std::tr1::shared_ptr<Session> addSession(const NetworkAddress& address);

    time_t currentTime() const;

	uint16_t port() const;
protected:
    typedef std::map<NetworkAddress, std::tr1::shared_ptr<Session>> SessionMap;
	SessionMap sessions_;	

    Protocol<uint32_t> protocol_;

private:	
	SocketServer();
    SocketServer(const SocketServer&);
    SocketServer& operator=(const SocketServer&);

	virtual void onUpdate() = 0;
    virtual void initializeProtocol() = 0;

    std::tr1::shared_ptr<SocketServerImpl> pimpl_;

	time_t current_time_;
    time_t last_cleanup_time_;
};

#endif // OPENSWG_SOCKET_SERVER_H