/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SANDBOX_ZONE_ZONE_SERVER_H_
#define SANDBOX_ZONE_ZONE_SERVER_H_

#include "shared/galaxy_server.h"

namespace sandbox {
namespace zone {

class ZoneServer : public shared::GalaxyServer
{
public:
    explicit ZoneServer(uint16_t port);
    virtual ~ZoneServer();
    
private:
	ZoneServer();
    ZoneServer(const ZoneServer&);
    ZoneServer& operator=(const ZoneServer&);

    virtual void onIncoming(const shared::NetworkAddress& address, shared::ByteBuffer& message);
    virtual void onUpdate();
    virtual void initializeProtocol();
};

}  // namespace zone
}  // namespace sandbox

#endif  // SANDBOX_ZONE_ZONE_SERVER_H_
