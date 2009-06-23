/**
 * *********************************************************************
 * OpenSWG Sandbox Server
 * Copyright (C) 2006 OpenSWG Team <http://www.openswg.com>
 * *********************************************************************
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
 */

#include <string>
#include "LoginHandler.h"
#include "GalaxySession.h"
#include "PacketTools.h"

void HandleAuthentication(GalaxySession *session, char *data, unsigned short length)
{
	// Send the station identifier
	session->SendHardPacket("packets\\Login\\StationIdentifier.txt", false);

	// Send the server list.
	session->SendHardPacket("packets\\Login\\ServerList.txt", false);

	// Send the server ip list.
	//session->SendHardPacket("packets\\Login\\ServerIpList.txt", false);

	unsigned short ipListSize;
	char *ipList = loadPacket("packets\\Login\\ServerIpList.txt", &ipListSize);

    std::string galaxyIp = "127.0.0.1"; // @todo replace with a call to configuration.

	char *tmp = new char[ipListSize+galaxyIp.length()+4];
	memcpy(tmp, ipList, 18); // Copy in the beginning of the packet.

	// Add the connection id to the packet data.
	unsigned int *ptr	= (unsigned int*)(tmp+18);
	*ptr = (uint16_t)galaxyIp.length();

	ptr = (unsigned int*)(tmp+20);
	memcpy(ptr, galaxyIp.c_str(), galaxyIp.length());

	memcpy(tmp+20+galaxyIp.length(), ipList+18, 30); // Copy in the end of the packet.

	session->SendHardPacket(tmp, ipListSize+galaxyIp.length()+3, false);
	
	// Send the character list.
	session->SendHardPacket("packets\\Login\\ServerCharacterList.txt", false);
}