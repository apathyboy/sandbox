/**
 * *********************************************************************
 * OpenSWG Sandbox Server
 * Copyright (C) 2006 OpenSWG Team <http://www.openswg.com>
 * *********************************************************************
 * Copyright (C) 2006 Team SWGEmu <http://www.swgemu.com>
 * ********************************************************************* *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * To read the license please visit http://www.gnu.org/copyleft/gpl.html
 * *********************************************************************
 *
 * All Encryption and Compression functions by Team SWGEmu!
 * Packet Loading Function by OpenSWG Team
 */

#ifndef PACKET_TOOLS_H
#define PACKET_TOOLS_H

#include <vector>
#include <tr1/memory>

#include "ByteBuffer.h"

std::tr1::shared_ptr<ByteBuffer> LoadPacketFromTextFile(const std::string& name);

void Compress(std::tr1::shared_ptr<ByteBuffer> packet);
void Decompress(std::tr1::shared_ptr<ByteBuffer> packet);

void Encrypt(std::tr1::shared_ptr<ByteBuffer> packet, uint32_t seed);
void AppendCrc(std::tr1::shared_ptr<ByteBuffer> packet, uint32_t seed, uint16_t seedLength = 2);

char* loadPacket(const std::string& name, unsigned short* length);
uint8_t axtoi(const char * const hexString);

/** SWGEmu Code Below This Point
 *	Copyright (C) 2006 Team SWGEmu <http://www.swgemu.com>
 */

static const int CompBuf = 800;

unsigned int GenerateCrc(char* pData,unsigned short nLength,unsigned int nCrcSeed);
bool CrcTest(char *pData,unsigned short nLength,unsigned int nCrcSeed, short nCrcLength=2);
void AppendCRC(char *pData,unsigned short nLength,unsigned int nCrcSeed, short nCrcLength=2);

void Encrypt(char *pData,unsigned short nLength,unsigned int nCRCSeed);
void Decrypt(char *pData,unsigned short nLength, unsigned int nCrcSeed);

char *Decompress(char *pData, unsigned short &nLength);
std::vector<char> Decompress(std::vector<char> pData);

#endif