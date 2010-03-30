/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
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