/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SRC_SHARED_LOGIN_HANDLER_H_
#define SRC_SHARED_LOGIN_HANDLER_H_

class Session;
class ByteBuffer;

void HandleAuthentication(Session& session, ByteBuffer& message);
void HandleSession(Session& session, ByteBuffer& message);

#endif // SRC_SHARED_LOGIN_HANDLER_H_