/*
 * OpenSWG Combat Upgrade Sandbox - LoginServer.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
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