/*
 * OpenSWG Combat Upgrade Sandbox - ZoneServer.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "SocketServer.h"

class ZoneServer : public SocketServer
{
public:
    explicit ZoneServer(uint16_t port);
    virtual ~ZoneServer();

private:
	ZoneServer();
    ZoneServer(const ZoneServer&);
    ZoneServer& operator=(const ZoneServer&);

    virtual void onUpdate();
    void sendShuttleUpdate();
    
	uint8_t  shuttle_state_;
};