/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include <gtest/gtest.h>

#include <iostream>
#include <fstream>

#include "shared/packet_tools.h"

using ::sandbox::shared::ByteBuffer;
using ::sandbox::shared::LoadPacketFromTextFile;

namespace {

TEST(LoadPacketFromTextFileTests, CanFindSingleHexString)
{
    std::ofstream outfile("CanFindSingleHexString.txt");
    outfile << "0x01";
    outfile.close();

    std::unique_ptr<ByteBuffer> packet(
      LoadPacketFromTextFile("CanFindSingleHexString.txt"));
    std::remove("CanFindSingleHexString.txt");

    EXPECT_EQ(1, packet->size());
    EXPECT_EQ(1, packet->read<uint8_t>());
}

TEST(LoadPacketFromTextFileTests, CanFindTwoHexStrings)
{
    std::ofstream outfile("CanFindTwoHexStrings.txt");
    outfile << "0x01 0x02";
    outfile.close();

    std::unique_ptr<ByteBuffer> packet(
      LoadPacketFromTextFile("CanFindTwoHexStrings.txt"));
    std::remove("CanFindTwoHexStrings.txt");

    EXPECT_EQ(2, packet->size());
    EXPECT_EQ(1, packet->read<uint8_t>());
    EXPECT_EQ(2, packet->read<uint8_t>());
}

}
