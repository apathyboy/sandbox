/*
 * SWG:ANH Sandbox Tests
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include "shared/session_manager.h"

#include <gtest/gtest.h>

namespace {

TEST(SessionManagerTests, NewManagerHasNoActiveSessions) {
    sandbox::shared::SessionManager manager;

    EXPECT_EQ(0, manager.CountSessions());
}

}  // namespace
