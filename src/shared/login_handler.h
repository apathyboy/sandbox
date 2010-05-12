/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SRC_SHARED_LOGIN_HANDLER_H_
#define SRC_SHARED_LOGIN_HANDLER_H_

namespace sandbox {
namespace shared {

class Session;
class ByteBuffer;

void HandleAuthentication(Session& session,
                          sandbox::shared::ByteBuffer& message);

void HandleSession(Session& session, sandbox::shared::ByteBuffer& message);

}  // namespace sandbox
}  // namespace shared

#endif  // SRC_SHARED_LOGIN_HANDLER_H_
