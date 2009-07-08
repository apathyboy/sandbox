/*
 * OpenSWG: GalaxyEngine - GalaxyServer.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef GALAXY_SERVER_H_
#define GALAXY_SERVER_H_

#include "UdpSocketListener.h"
#include "Protocol.h"
#include "NetworkAddress.h"

class Session;

class GalaxyServer
{
public:
    explicit GalaxyServer(uint16_t port);
    virtual ~GalaxyServer();

    uint16_t port();
    void     port(uint16_t);

	void run();

    void handleIncoming(const NetworkAddress& address, ByteBuffer& message);
    void sendToRemote(const NetworkAddress& address, ByteBuffer& message) const;

    std::tr1::shared_ptr<Session> addSession(const NetworkAddress& address);
    std::tr1::shared_ptr<Session> findSession(const NetworkAddress& address);
    std::tr1::shared_ptr<Session> removeSession(const NetworkAddress& address);
    uint32_t sessionCount() const;


private:
    /* Disable compiler generated methods */
    GalaxyServer();
  
    GalaxyServer(const GalaxyServer&);
    GalaxyServer& operator=(const GalaxyServer&);

    /* SOE level handlers */
    void handleSessionRequest(const NetworkAddress& address, ByteBuffer& message);
    void handleNetStatus(const NetworkAddress& address, ByteBuffer& message);
    void handleMultiPacket(const NetworkAddress& address, ByteBuffer& message);
    void handleAcknowledge(const NetworkAddress& address, ByteBuffer& message);
    void handleDataChannel(const NetworkAddress& address, ByteBuffer& message);
    void handleDisconnect(const NetworkAddress& address, ByteBuffer& message);
    void handlePing(const NetworkAddress& address, ByteBuffer& message);

    UdpSocketListener   network_listener_;

    typedef std::tr1::function<void (const NetworkAddress& address, ByteBuffer&)> SoeMessageHandler;
    Protocol<uint16_t, SoeMessageHandler>  soe_protocol_;
    Protocol<uint32_t>  swg_protocol_;

    typedef std::map<NetworkAddress, std::tr1::shared_ptr<Session>> SessionMap;
	SessionMap sessions_;	
    
    virtual void initializeProtocol() = 0;

    virtual void onIncoming(const NetworkAddress& address, ByteBuffer& message) = 0;
	virtual void onUpdate() = 0;
};

#endif // GALAXY_SERVER_H_

