/**
 * Legacy Sandbox Project - TestLoadPacketFromTextFile.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include <gtest/gtest.h>

#include <iostream>
#include <fstream>

#include "PacketTools.h"

namespace {

    TEST(LoadPacketFromTextFileTests, CanFindSingleHexString)
    {
        std::ofstream outfile("CanFindSingleHexString.txt");
        outfile << "0x01";
        outfile.close();

        std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("CanFindSingleHexString.txt");
        remove("CanFindSingleHexString.txt");

        EXPECT_EQ(1, packet->size());
        EXPECT_EQ(1, packet->read<uint8_t>());
    }

    TEST(LoadPacketFromTextFileTests, CanFindTwoHexStrings)
    {
        std::ofstream outfile("CanFindTwoHexStrings.txt");
        outfile << "0x01 0x02";
        outfile.close();

        std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("CanFindTwoHexStrings.txt");
        remove("CanFindTwoHexStrings.txt");

        EXPECT_EQ(2, packet->size());
        EXPECT_EQ(1, packet->read<uint8_t>());
        EXPECT_EQ(2, packet->read<uint8_t>());
    }

}