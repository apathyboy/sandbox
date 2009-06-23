/**
 * Legacy Sandbox Project - TestByteBuffer.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include <gtest/gtest.h>

#include "ByteBuffer.h"

namespace {

TEST(ByteBufferTests, ByteBufferIsEmptyWhenCreated)
{
	ByteBuffer buffer;
	EXPECT_EQ(0, buffer.size());
}

TEST(ByteBufferTests, ByteBufferDefaultCapacityIsZero)
{
	ByteBuffer buffer;
	EXPECT_EQ(0, buffer.capacity());
}

TEST(ByteBufferTests, WritingIntReportsCorrectSizeAndCapacity)
{
	ByteBuffer buffer;
	buffer.write<int>(10);

	EXPECT_EQ(4, buffer.size());
	EXPECT_EQ(4, buffer.capacity());
}

TEST(ByteBufferTests, WritingTwoIntsReportsCorrectSizeAndCapacity)
{
	ByteBuffer buffer;

	buffer.write<int>(10);
	EXPECT_EQ(4, buffer.size());
	EXPECT_EQ(4, buffer.capacity());

	buffer.write<int>(20);
	EXPECT_EQ(8, buffer.size());
	EXPECT_EQ(8, buffer.capacity());
}

TEST(ByteBufferTests, CanReadIntWrittenToTheBuffer)
{
	ByteBuffer buffer;

	buffer.write<int>(10);
	EXPECT_EQ(10, buffer.read<int>());
}

TEST(ByteBufferTests, CanReadTwoIntsWrittenToTheBuffer)
{
	ByteBuffer buffer;

	buffer.write<int>(10);
	buffer.write<int>(20);

	EXPECT_EQ(10, buffer.read<int>());
	EXPECT_EQ(20, buffer.read<int>());
}

TEST(ByteBufferTests, ReadingPastBufferEndThrowsException)
{
	ByteBuffer buffer;
	EXPECT_EQ(0, buffer.size());

	EXPECT_THROW(buffer.read<int>(), std::out_of_range);
}

TEST(ByteBufferTests, WritingStringReportsCorrectSizeAndCapacity)
{
	ByteBuffer buffer;
	EXPECT_EQ(0, buffer.size());
	EXPECT_EQ(0, buffer.capacity());

	buffer.write<std::string>(std::string("test string"));

	EXPECT_EQ(13, buffer.size());
	EXPECT_EQ(13, buffer.capacity());
}

TEST(ByteBufferTests, CanReadStringWrittenToTheBuffer)
{
	ByteBuffer buffer;
	std::string test_string("test string data");

	buffer.write<std::string>(test_string);

	EXPECT_EQ(test_string, buffer.read<std::string>());
}

TEST(ByteBufferTests, CanReadTwoStringsWrittenToTheBuffer)
{
	ByteBuffer buffer;
	std::string test_string1("first test string");
	std::string test_string2("second test string");

	buffer.write<std::string>(test_string1);
	buffer.write<std::string>(test_string2);

	EXPECT_EQ(test_string1, buffer.read<std::string>());
	EXPECT_EQ(test_string2, buffer.read<std::string>());
}

TEST(ByteBufferTests, WritingUnicodeStringReportsCorrectSizeAndCapacity)
{
	ByteBuffer buffer;
	EXPECT_EQ(0, buffer.size());
	EXPECT_EQ(0, buffer.capacity());

	buffer.write<std::wstring>(std::wstring(L"test string"));

	// Length and Capacity should be size of int + size of string * size of wchar_t.
	EXPECT_EQ(sizeof(uint32_t) + (11 * 2), buffer.size());
	EXPECT_EQ(sizeof(uint32_t) + (11 * 2), buffer.capacity());
}

TEST(ByteBufferTests, CanReadUnicodeStringWrittenToTheBuffer)
{
	ByteBuffer buffer;
	std::wstring test_string(L"test string data");

	buffer.write<std::wstring>(test_string);
	EXPECT_EQ(test_string, buffer.read<std::wstring>());
}

TEST(ByteBufferTests, CanClearBufferData)
{
	ByteBuffer buffer;
	EXPECT_EQ(0, buffer.size());

	buffer.write<int>(3);
	EXPECT_EQ(4, buffer.size());

	buffer.clear();
	EXPECT_EQ(0, buffer.size());
}

TEST(ByteBufferTests, CanStreamData)
{
	ByteBuffer buffer;
	int testInt = 3;

	buffer << testInt;
	EXPECT_EQ(3, buffer.read<int>());
}

TEST(ByteBufferTests, PeekingDataDoesNotMoveReadPosition)
{
	ByteBuffer buffer;
	buffer.write<int>(3);
	buffer.write<int>(10);

	// Peek the data twice, this should return the same integer value (3) both times.
	EXPECT_EQ(3, buffer.peek<int>());
	EXPECT_EQ(3, buffer.peek<int>());
}

TEST(ByteBufferTests, CanReadAndWriteUnsignedData)
{
	ByteBuffer buffer;
	buffer.write<uint16_t>(3);

	EXPECT_EQ(2, buffer.size()); // First check that the right datasize was written.
	EXPECT_EQ(3, buffer.read<uint16_t>()); // Check that the value is correct.
}

}