/*
 * OpenSWG: GalaxyEngine - UdpSocketListener.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef UDP_SOCKET_LISTENER_H_
#define UDP_SOCKET_LISTENER_H_

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <tr1/functional>
#include <tr1/memory>

#include "NetworkAddress.h"

class ByteBuffer;

class UdpSocketListener
{
public:
    typedef std::tr1::function<void (const NetworkAddress&, ByteBuffer&)> Callback;

    UdpSocketListener(uint16_t port);
    ~UdpSocketListener();

    uint16_t port() const;
    void     port(uint16_t port);

    Callback callback();
    void     callback(Callback callback);

    void poll();
    void sendToRemote(const NetworkAddress& address, ByteBuffer& message) const;

private:
    UdpSocketListener();
    UdpSocketListener(const UdpSocketListener&);
    UdpSocketListener& operator=(const UdpSocketListener&);

    class UdpSocketListenerImpl;
    std::tr1::shared_ptr<UdpSocketListenerImpl> impl_;
};

#endif // UDP_SOCKET_LISTENER_H_

