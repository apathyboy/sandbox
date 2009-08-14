/*
 * OpenSWG Combat Upgrade Sandbox - NetworkAddress.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

 #ifndef NETWORKADDRESS_H_
 #define NETWORKADDRESS_H_

 #include <boost/asio.hpp>

typedef boost::asio::ip::udp::endpoint NetworkAddress;

 #endif // NETWORKADDRESS_H_