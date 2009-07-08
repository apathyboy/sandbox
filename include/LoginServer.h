/*
 * OpenSWG Combat Upgrade Sandbox - LoginServer.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "SocketServer.h"

class LoginServer : public SocketServer
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

    Protocol<uint32_t> protocol_;
};