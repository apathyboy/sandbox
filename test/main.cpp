/*
 * OpenSWG Combat Upgrade Sandbox - main.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include <iostream>
#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
	std::cout << "Running sandbox unit tests\n";

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}