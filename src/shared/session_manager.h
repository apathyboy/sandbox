/**
 * SWG:ANH Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
 */

#ifndef SRC_SHARED_SESSION_MANAGER_H_
#define SRC_SHARED_SESSION_MANAGER_H_

#include <cstdint>

namespace sandbox {
namespace shared {

class SessionManager {
 public:
  SessionManager();
  ~SessionManager();

  uint16_t CountSessions() const;

 private:
  // Disable default copy and assignment.
  SessionManager(const SessionManager&);
  void operator=(const SessionManager&);
};

}  // namespace shared
}  // namespace sandbox

#endif  // SRC_SHARED_SESSION_MANAGER_H_
