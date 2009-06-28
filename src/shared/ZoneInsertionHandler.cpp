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

#include "ZoneInsertionHandler.h"
#include "GalaxySession.h"
#include "PacketTools.h"


void HandleClientReady(GalaxySession *session, const unsigned char * data, unsigned short length)
{
	session->sendHeartbeat();
}

void HandleLoadDone(GalaxySession *session, const unsigned char * data, unsigned short length)
{
	session->sendHardcodedPacket("packets\\ZoneInsertion\\LoadDone.txt", true);
}

void HandleLoadTerrain(GalaxySession *session, const unsigned char * data, unsigned short length)
{
	session->sendHeartbeat();
}

void HandleSession(GalaxySession *session, const unsigned char * data, unsigned short length)
{
	session->sendHeartbeat();

	// Send the character options to the client.
	session->sendHardcodedPacket("packets\\ZoneInsertion\\CharacterOptions.txt", false);
}

void HandleZoneInsertionRequest(GalaxySession *session, const unsigned char * data, unsigned short length)
{
	session->sendHeartbeat();

	// Send the initial batch of packets.
	session->sendHardcodedPacket("packets\\ZoneInsertion\\UnkByteFlag.txt", false);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Int900Pkt.txt", false);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\LoadTerrain.txt", true);

	// Begin loading the character.
	session->sendHardcodedPacket("packets\\ZoneInsertion\\PreloadCharacter.txt", false);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\CreoCreate.txt", false);
	//session->SendHardPacket("packets\\ZoneInsertion\\PrecloseClose.txt", false);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Creo3.txt", true);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Creo6.txt", true);

	session->sendHardcodedPacket("packets\\ZoneInsertion\\Creo1.txt", true);

	// @TODO Implement a better way to send hardcoded fragmented packets.
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Creo4.txt", true);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Creo4pt2.txt", true);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Creo4pt3.txt", true);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Creo4pt4.txt", true);

	// Create our character.
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Creo8.txt", true);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Play8.txt", true);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Play9.txt", false);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\PrecloseClose.txt", false);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\PlayClose.txt", false);
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Play9Update.txt", false);

	// Inventory
	session->sendHardcodedPacket("packets\\ZoneInsertion\\Inventory.txt", false);

	// Environment
	session->sendHardcodedPacket("packets\\Objects\\KerenStarship.txt", false);
	session->sendHardcodedPacket("packets\\Objects\\KerenStarshipTicketCollector.txt", false);
}