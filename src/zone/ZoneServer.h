/*
 * OpenSWG Combat Upgrade Sandbox - ZoneServer.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "GalaxyServer.h"

class ZoneServer : public GalaxyServer
{
public:
    explicit ZoneServer(uint16_t port);
    virtual ~ZoneServer();
    
private:
	ZoneServer();
    ZoneServer(const ZoneServer&);
    ZoneServer& operator=(const ZoneServer&);

    virtual void onIncoming(const NetworkAddress& address, ByteBuffer& message);
    virtual void onUpdate();
    virtual void initializeProtocol();
};