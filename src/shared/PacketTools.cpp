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


#include "PacketTools.h"
#define MAX_PACKET_SIZE 1000

#include <malloc.h>
#include <zlib.h>

#ifdef WIN32		
#include <tchar.h>
#endif
		
#include <fstream>
#include <string>

char* loadPacket(char* const name, unsigned short* length = NULL) 
{
	// Create a container for the packet data and a buffer
	// to use when reading in the file data.
 	std::string data;
	std::string buffer;
		
	// Remove leading and trailing whitespace
	static const char whitespace[] = " \n\t\v\r\f";

	// Read in the file a line at a time.
	std::ifstream ifs(name);
	while (std::getline(ifs, buffer)) {
		// Remove any comments.
		buffer = buffer.substr( 0, buffer.find('#'));	
		
		// Remove any whitespace from the beginning/end of the line.
		buffer.erase(0, buffer.find_first_not_of(whitespace));
		buffer.erase(buffer.find_last_not_of(whitespace) + 1U);

		// Append the buffer to the packet data.
		data.append(buffer);
	}

	char* packet;				// Container for the final packet data.
	char* tokenPtr;				// Tokenizer for breaking up the string.
	char tmp[MAX_PACKET_SIZE]; // Buffer 
	*length = 0;				// Initialize the packet size.

	// This for loop breaks apart the packet data string and converts
	// each string value to is hexidecimal value and stores the
	// resulting data in the buffer. Also, at the end of the for loop
	// the next token pointer is found and the length is increased.
	for (tokenPtr = strtok(const_cast<char*>(data.c_str()), " ,\n");
		 tokenPtr;
		 tokenPtr = strtok(NULL, " ,\n"), *length += 1)
	{		
		*(unsigned char*)(tmp+*length) = (unsigned char)axtoi(tokenPtr+2);
	}

	// Copy the packet data from the buffer to the packet container.
	packet = new char[*length];
	for (int j = 0; j < *length; j++)
	{
		*(unsigned char*)(packet+j) = (unsigned char)*(tmp+j);
	}
	
	/* Uncomment this to display an output of each packet as it's loaded.
	printf("LOADED PACKET\nPacket Size: %d \nPacket Data: \n", *length);
	for(int k = 0; k < *length; k++)
	{
		printf("0x%02x ",(unsigned char)*(packet+k));
	}
	printf("\n");
	*/

	return packet;
}

int axtoi(char *hexStg) {
	int n = 0;         // position in string
	int m = 0;         // position in digit[] to shift
	int count;         // loop index
	int intValue = 0;  // integer value of hex string
	int digit[5];      // hold values to convert
	while (n < 4) {
		if (hexStg[n]=='\0')
			break;
		if (hexStg[n]!='\n'&&hexStg[n]!=' '&&hexStg[n]!='\a') {
			if (hexStg[n] > 0x2f && hexStg[n] < 0x40 ) //if 0 to 9
				digit[n] = hexStg[n] & 0x0f;            //convert to int
			else if (hexStg[n] >='a' && hexStg[n] <= 'f') //if a to f
				digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
			else if (hexStg[n] >='A' && hexStg[n] <= 'F') //if A to F
				digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
			else break;
		}
		n++;
	}
	
	count = n;
	m = n - 1;
	n = 0;
	
	while(n < count) {
		// digit[n] is value of hex digit at position n
		// (m << 2) is the number of positions to shift
		// OR the bits into return value
		intValue = intValue | (digit[n] << (m << 2));
		m--;   // adjust the position to set
		n++;   // next digit to process
	}
  
	return (intValue);
}


/** SWGEmu Code Below This Point
 *	Copyright (C) 2006 Team SWGEmu <http://www.swgemu.com>
 */

unsigned int GenerateCrc(char* pData,unsigned short nLength,unsigned int nCrcSeed)
{
    unsigned int nCrc = g_nCrcTable[(~nCrcSeed) & 0xFF];
    nCrc ^= 0x00FFFFFF;
    unsigned int nIndex = (nCrcSeed >> 8) ^ nCrc;
    nCrc = (nCrc >> 8) & 0x00FFFFFF;
    nCrc ^= g_nCrcTable[nIndex & 0xFF];
    nIndex = (nCrcSeed >> 16) ^ nCrc;
    nCrc = (nCrc >> 8) & 0x00FFFFFF;
    nCrc ^= g_nCrcTable[nIndex & 0xFF];
    nIndex = (nCrcSeed >> 24) ^ nCrc;
    nCrc = (nCrc >> 8) &0x00FFFFFF;
    nCrc ^= g_nCrcTable[nIndex & 0xFF];

    for( short i = 0; i < nLength; i++ )
    {
        nIndex = (pData[i]) ^ nCrc;
        nCrc = (nCrc >> 8) & 0x00FFFFFF;
        nCrc ^= g_nCrcTable[nIndex & 0xFF];
    }
    return ~nCrc;
}


bool CrcTest(char *pData, unsigned short nLength,unsigned int nCrcSeed, short nCrcLength)
{
    bool crctest = true;
    if(nCrcLength > 0)
    {
        unsigned int p_crc = GenerateCrc(pData,(nLength-nCrcLength),nCrcSeed);
        unsigned int  crc = 0;
        unsigned int mask = 0;
        unsigned int pullbyte = 0;
        pData = pData + (nLength-nCrcLength);
        for( short i = 0; i < nCrcLength; i++ )
        {
            pullbyte = (unsigned char)pData[i];
            crc |=(pullbyte << (((nCrcLength - 1) - i) * 8));
            mask <<= 8;
            mask |= 0xFF;
        }
        p_crc &= mask;
        if(p_crc != crc)
            crctest = false;
    }
    return crctest;
}

void AppendCRC(char *pData, unsigned short nLength, unsigned int nCrcSeed, short nCrcLength)
{
    if (nCrcLength > 0)
    {
        unsigned int crc = GenerateCrc(pData,(nLength-nCrcLength),nCrcSeed);
        pData += (nLength-nCrcLength);
        for( short i = 0; i < nCrcLength; i++ )
        {
            pData[(nCrcLength - 1) - i] = (char)((crc >> (8 * i)) & 0xFF);
        }
    }

}


void Decrypt(char *pData,unsigned short nLength,unsigned int nCrcSeed)
{
    unsigned int *Data;
    if(pData[0] == 0x00)
    {
     nLength-=4;
     Data = (unsigned int*)(pData+2);
    }
    else
    {
     nLength-=3;
     Data = (unsigned int*)(pData+1);
    }
    short block_count = (nLength / 4);
    short byte_count = (nLength % 4);
    unsigned int itemp;
    for(short count = 0;count<block_count;count++)
    {
        itemp = *Data;
        *Data ^= nCrcSeed;
        nCrcSeed = itemp;
        Data++;
    }
    pData = (char*)Data;
    for(short count = 0;count<byte_count;count++)
    {
        *pData ^= nCrcSeed;
        pData++;
    }
}

void Encrypt(char *pData, unsigned short nLength,unsigned int nCrcSeed)
{
    unsigned int *Data;
    if(pData[0] == 0x00)
    {
     nLength-=4;
     Data = (unsigned int*)(pData+2);
    }
    else
    {
     nLength-=3;
     Data = (unsigned int*)(pData+1);
    }
    short block_count = (nLength / 4);
    short byte_count = (nLength % 4);
 
    for(short count = 0;count<block_count;count++)
    {
        *Data ^= nCrcSeed;
        nCrcSeed = *Data;
        Data++;
    }
    pData = (char*)Data;
    for(short count = 0;count<byte_count;count++)
    {
        *pData ^= (char)nCrcSeed;
        pData++;
    }
}

char *Decompress(char *pData, unsigned short &nLength)
{
    
    unsigned short offset;
    if(pData[0] == 0x00)
     offset = 2;
    else
     offset = 1;
    z_stream packet;
    char output[CompBuf];
    unsigned short newLength=0;
    packet.zalloc = Z_NULL;
    packet.zfree = Z_NULL;
    packet.opaque = Z_NULL;
    packet.avail_in = 0;
    packet.next_in = Z_NULL;
    inflateInit(&packet);
    packet.next_in = (Bytef*)(pData+offset);
    packet.avail_in = (nLength - offset -3);
    packet.next_out = (Bytef*)output;
    packet.avail_out = CompBuf;
    inflate(&packet,Z_FINISH);
    newLength = static_cast<unsigned short>(packet.total_out);
    inflateEnd(&packet);
    char *Decomp_pData  = new char [newLength + offset + 3];
    char *begDecomp_pData = Decomp_pData;
    *Decomp_pData = pData[0];
    Decomp_pData++;
    if(offset == 2)
	{
	 *Decomp_pData = pData[1];
     Decomp_pData++;
	}
    for(short x=0;x<newLength;x++)
    {
        *Decomp_pData = output[x];
        Decomp_pData++;
    }
	*Decomp_pData = 0x01;
    Decomp_pData++;
	pData += (nLength-2);
    *(unsigned short*)Decomp_pData = *(unsigned short*)pData;
    Decomp_pData = begDecomp_pData;
    nLength = newLength + offset + 3;
    return Decomp_pData;

}

char *Compress(char *pData, unsigned short &nLength)
{
 unsigned short offset;
 if(pData[0] == 0x00)
   offset = 2;
 else
   offset = 1;
 z_stream packet;
 char *output = new char[nLength+20];
 packet.zalloc = Z_NULL;
 packet.zfree = Z_NULL;
 packet.opaque = Z_NULL;
 packet.avail_in = 0;
 packet.next_in = Z_NULL;
 deflateInit(&packet,Z_DEFAULT_COMPRESSION);
 packet.next_in = (Bytef* )(pData+offset);
 packet.avail_in = nLength - offset - 3;
 packet.next_out = (Bytef* )output;
 packet.avail_out = nLength + 20;
 deflate(&packet,Z_FINISH);
 unsigned short newLength = static_cast<unsigned short>(packet.total_out);
 deflateEnd(&packet);
 char *comp_pData  = new char [newLength+ offset + 3];
 char *begcomp_pData;
 begcomp_pData = comp_pData;
 *comp_pData = pData[0];
 comp_pData++;
 if(offset == 2)
 {
   *comp_pData = pData[1];
   comp_pData++;
 }
 for(short x=0;x<newLength;x++)
 {
  *comp_pData = output[x];
  comp_pData++;
  }
 *comp_pData = 0x01;
 comp_pData++;
 pData += (nLength-2);
 *(unsigned short*)comp_pData = *(unsigned short*)pData;
 comp_pData = begcomp_pData;
 nLength = newLength + offset + 3;
 return comp_pData;
}