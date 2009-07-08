/*
 * OpenSWG Combat Upgrade Sandbox - PacketTools.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef PACKET_TOOLS_H
#define PACKET_TOOLS_H

#include <vector>
#include <tr1/memory>

#include "ByteBuffer.h"

ByteBuffer LoadPacketFromTextFile(const std::string& name);

void Compress(ByteBuffer& packet);
void Decompress(ByteBuffer& packet);

void Encrypt(ByteBuffer& packet, uint32_t seed, uint16_t seedLength = 2);
void Decrypt(ByteBuffer& packet, uint32_t seed, uint16_t seedLength = 2);

uint32_t GenerateCrc(ByteBuffer& packet, uint32_t seed, uint16_t seedLength = 2);
bool CrcTest(ByteBuffer& packet, uint32_t seed, uint16_t seedLength = 2);
void AppendCrc(ByteBuffer& packet, uint32_t seed, uint16_t seedLength = 2);

uint8_t axtoi(const char * const hexString);

#endif

