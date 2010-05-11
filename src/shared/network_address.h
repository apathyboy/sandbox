/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

 #ifndef NETWORKADDRESS_H_
 #define NETWORKADDRESS_H_

 #include <boost/asio.hpp>

namespace sandbox {
namespace shared {

typedef boost::asio::ip::udp::endpoint NetworkAddress;

}  // namespace sandbox
}  // namespace shared

 #endif // NETWORKADDRESS_H_