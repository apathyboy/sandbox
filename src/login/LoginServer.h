/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include "GalaxyServer.h"

class LoginServer : public GalaxyServer
{
public:
    explicit LoginServer(uint16_t port);
    virtual ~LoginServer();

private:
	LoginServer();
    LoginServer(const LoginServer&);
    LoginServer& operator=(const LoginServer&);

    virtual void onIncoming(const NetworkAddress& address, ByteBuffer& message);
	virtual void onUpdate();
    virtual void initializeProtocol();
};