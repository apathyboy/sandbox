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
#include <regex>

std::tr1::shared_ptr<ByteBuffer> LoadPacketFromTextFile(const std::string& name)
{
    std::string line_buffer;
    std::tr1::shared_ptr<ByteBuffer> packet(new ByteBuffer());

    static const std::tr1::regex pattern("0x([0-9a-fA-F]+)");
    const int keep[] = {1}; 
    std::tr1::smatch result;

    std::ifstream file_stream(name.c_str());
    while (std::getline(file_stream, line_buffer)) {
        
        // Loop over the line searching for the pattern. Note the "keep"
        // is used to select the 1st subpattern to keep. Leaving this off
        // matches the 0x as well.
        const std::tr1::sregex_token_iterator end;
        for (std::tr1::sregex_token_iterator i(
                line_buffer.begin(), line_buffer.end(), pattern, keep); 
             i != end; ++i) {
                 *packet << axtoi((*i).str().c_str());
        }
    }

    return packet;
}


void Compress(std::tr1::shared_ptr<ByteBuffer> packet)
{
    // Grab a reference to the internals of the packet. Generally
    // this should not be done but this is one of the special circumstances
    // the raw() was implemented for. This allows us to work on
    // the raw data with a non-standard library and minimize the amount
    // of copying.
    std::vector<uint8_t>& packet_data = packet->raw();  

    // Determine the offset to begin compressing data at.
    uint16_t offset = (packet_data[0] == 0x00) ? 2 : 1;

    // Create a container for the compressed data and initialize the 
    // z_stream with the necessary default values.
    z_stream stream;

    stream.zalloc   = Z_NULL;
    stream.zfree    = Z_NULL;
    stream.opaque   = Z_NULL;
    stream.avail_in = 0;
    stream.next_in  = Z_NULL;

    deflateInit(&stream, Z_DEFAULT_COMPRESSION); 
 
    // Prepare the stream for compression and then compress the data.
    std::vector<uint8_t> compression_output(packet->size() + 20);
 
    stream.next_in   = reinterpret_cast<Bytef *>(&packet_data[offset]);
    stream.avail_in  = packet->size() - offset - 3;
    stream.next_out  = reinterpret_cast<Bytef *>(&compression_output[0]);
    stream.avail_out = packet->size() + 20;

    deflate(&stream, Z_FINISH);

    // SOE protocol requires this bit at the end of compressed data.
    compression_output[stream.total_out] = 0x01;

    // Replace the section of the packet data copy between the offset and 
    // the crc bits.
    packet_data.insert(
        packet_data.erase(packet_data.begin() + offset, packet_data.end() - 2),
        compression_output.begin(),
        compression_output.begin() + stream.total_out + 1 // The +1 is for the added SOE bit.
        );
}

void Encrypt(std::tr1::shared_ptr<ByteBuffer> packet, uint32_t seed)
{
    std::vector<uint8_t> packet_data(packet->data(), packet->data() + packet->size());
    char* pData = reinterpret_cast<char*>(&packet_data[0]);

    uint16_t nLength = packet->size();

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
        seed ^= *Data;
        *Data = seed;
        //*Data ^= nCrcSeed;
        //nCrcSeed = *Data;
        Data++;
    }
    pData = (char*)Data;
    for(short count = 0;count<byte_count;count++)
    {
        *pData ^= (char)seed;
        pData++;
    }

    ByteBuffer tmp(reinterpret_cast<uint8_t*>(&packet_data[0]), packet_data.size());
    packet->swap(tmp);
}

void AppendCrc(std::tr1::shared_ptr<ByteBuffer> packet, uint32_t seed, uint16_t seedLength)
{
    std::vector<char> pData(packet->data(), packet->data() + packet->size());
    uint16_t nLength = packet->size();

    if (seedLength > 0)
    {
        unsigned int crc = GenerateCrc(&pData[0], (nLength-seedLength), seed);
       // pData += (nLength-seedLength);
        for( short i = 0; i < seedLength; i++ )
        {
            pData[(nLength - 1) - i] = (char)((crc >> (8 * i)) & 0xFF);
        }
    }

    ByteBuffer tmp(reinterpret_cast<uint8_t*>(&pData[0]), pData.size());
    packet->swap(tmp);
}


char* loadPacket(const std::string& name, unsigned short* length = NULL) 
{
	// Create a container for the packet data and a buffer
	// to use when reading in the file data.
 	std::string data;
	std::string buffer;
		
	// Remove leading and trailing whitespace
	static const char whitespace[] = " \n\t\v\r\f";

	// Read in the file a line at a time.
    std::ifstream ifs(name.c_str());
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

uint8_t axtoi(const char * const hexString) {
	uint8_t n = 0;         // position in string
	uint8_t m = 0;         // position in digit[] to shift
	uint8_t count;         // loop index
	uint8_t intValue = 0;  // integer value of hex string
	uint8_t digit[5];      // hold values to convert
	while (n < 4) {
		if (hexString[n]=='\0')
			break;
		if (hexString[n]!='\n'&&hexString[n]!=' '&&hexString[n]!='\a') {
			if (hexString[n] > 0x2f && hexString[n] < 0x40 ) //if 0 to 9
				digit[n] = hexString[n] & 0x0f;            //convert to int
			else if (hexString[n] >='a' && hexString[n] <= 'f') //if a to f
				digit[n] = (hexString[n] & 0x0f) + 9;      //convert to int
			else if (hexString[n] >='A' && hexString[n] <= 'F') //if A to F
				digit[n] = (hexString[n] & 0x0f) + 9;      //convert to int
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
        nCrcSeed ^= *Data;
        *Data = nCrcSeed;
        //*Data ^= nCrcSeed;
        //nCrcSeed = *Data;
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

std::vector<char> Decompress(std::vector<char> pData)
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
    packet.next_in = (Bytef*)(&pData[0]+offset);
    packet.avail_in = (pData.size() - offset -3);
    packet.next_out = (Bytef*)output;
    packet.avail_out = CompBuf;
    inflate(&packet,Z_FINISH);
    newLength = static_cast<unsigned short>(packet.total_out);
    inflateEnd(&packet);
    
    std::vector<char> tmpdata(newLength + offset + 3);

    char *Decomp_pData  = &tmpdata[0];// new char [newLength + offset + 3];
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

    *(unsigned short*)Decomp_pData = *(reinterpret_cast<unsigned short*>(&pData[pData.size()-2]));
    Decomp_pData = begDecomp_pData;
    //nLength = newLength + offset + 3;
    return tmpdata;

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