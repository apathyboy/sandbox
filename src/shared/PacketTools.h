/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/


#ifndef PACKET_TOOLS_H
#define PACKET_TOOLS_H

#ifdef _MSC_VER
#include <memory>
#else
#include <tr1/memory>
#endif

#include <vector>

#include "ByteBuffer.h"

std::tr1::shared_ptr<ByteBuffer> LoadPacketFromTextFile(const std::string& name);

void Compress(ByteBuffer& packet);
void Decompress(ByteBuffer& packet);

void Encrypt(ByteBuffer& packet, uint32_t seed, uint32_t seedLength = 2);
void Decrypt(ByteBuffer& packet, uint32_t seed, uint32_t seedLength = 2);

uint32_t GenerateCrc(ByteBuffer& packet, uint32_t seed, uint32_t seedLength = 2);
bool CrcTest(ByteBuffer& packet, uint32_t seed, uint32_t seedLength = 2);
void AppendCrc(ByteBuffer& packet, uint32_t seed, uint32_t seedLength = 2);

uint8_t axtoi(const char * const hexString);

#endif

