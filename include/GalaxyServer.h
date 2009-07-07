/*
 * OpenSWG: GalaxyEngine - GalaxyServer.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef GALAXY_SERVER_H_
#define GALAXY_SERVER_H_

#include "UdpSocketListener.h"

class GalaxyServer
{
public:
    explicit GalaxyServer(uint16_t port);

    uint16_t port();
    void     port(uint16_t);

	void run();

    void handleIncoming(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> message);
    void sendToRemote(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> message) const;

    void tick(uint32_t delta);

private:
    /* Disable compiler generated methods */
    GalaxyServer();
  
    GalaxyServer(const GalaxyServer&);
    GalaxyServer& operator=(const GalaxyServer&);

    UdpSocketListener   network_listener_;
    Protocol            soe_protocol_;
    Protocol            swg_protocol_;
    
    virtual void initializeProtocol() = 0;

    virtual void onIncoming(const NetworkAddress& address, std::tr1::shared_ptr<ByteBuffer> message) = 0;
	virtual void onUpdate() = 0;
};

#endif // GALAXY_SERVER_H_

