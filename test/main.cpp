/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include <iostream>
#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
	std::cout << "Running sandbox unit tests\n";

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}