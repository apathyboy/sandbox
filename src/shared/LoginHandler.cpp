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

void HandleAuthentication(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message)
{
	// Send the station identifier
	session.sendHardcodedPacket("packets\\Login\\StationIdentifier.txt", false);

	// Send the server list.
	session.sendHardcodedPacket("packets\\Login\\ServerList.txt", false);

    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets\\Login\\ServerIpList.txt");
    
    std::string galaxy_ip = "127.0.0.1"; // @todo: Replace with a call to configuration.
    
    // Write the galaxy ip string length.
    packet->writeAt<uint16_t>(18, static_cast<uint16_t>(galaxy_ip.length()));    
    
    // Insert the galaxy ip string into the packet data.
    std::vector<uint8_t>& packet_data = packet->raw();  
    packet_data.insert(packet_data.begin() + 20, galaxy_ip.begin(), galaxy_ip.end());
    
    session.sendHardcodedPacket(packet, false);

	// Send the character list.
	session.sendHardcodedPacket("packets\\Login\\ServerCharacterList.txt", false);
}

