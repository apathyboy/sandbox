/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SRC_SHARED_NETWORK_ADDRESS_H_
#define SRC_SHARED_NETWORK_ADDRESS_H_

#include <boost/asio.hpp> // NOLINT

namespace sandbox {
namespace shared {

typedef boost::asio::ip::udp::endpoint NetworkAddress;

}  // namespace sandbox
}  // namespace shared

#endif  // SRC_SHARED_NETWORK_ADDRESS_H_
