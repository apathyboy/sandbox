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

std::tr1::shared_ptr<ByteBuffer> LoadPacketFromTextFile(const std::string& name);

void Compress(std::tr1::shared_ptr<ByteBuffer> packet);
void Decompress(std::tr1::shared_ptr<ByteBuffer> packet);

void Encrypt(std::tr1::shared_ptr<ByteBuffer> packet, uint32_t seed, uint16_t seedLength = 2);
void Decrypt(std::tr1::shared_ptr<ByteBuffer> packet, uint32_t seed, uint16_t seedLength = 2);

uint32_t GenerateCrc(std::tr1::shared_ptr<ByteBuffer> packet, uint32_t seed, uint16_t seedLength = 2);
bool CrcTest(std::tr1::shared_ptr<ByteBuffer> packet, uint32_t seed, uint16_t seedLength = 2);
void AppendCrc(std::tr1::shared_ptr<ByteBuffer> packet, uint32_t seed, uint16_t seedLength = 2);

char* loadPacket(const std::string& name, unsigned short* length);
uint8_t axtoi(const char * const hexString);

#endif

