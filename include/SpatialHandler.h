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

#ifndef OPENSWG_SPATIAL_HANDLER_H
#define OPENSWG_SPATIAL_HANDLER_H

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <tr1/memory>

class GalaxySession;
class ByteBuffer;

void HandleSpatial(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandlePositionUpdate(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleSit(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleStand(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleProne(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleKneel(GalaxySession& session, std::tr1::shared_ptr<ByteBuffer> message);

void HandleSpatial(GalaxySession *session, const unsigned char *data, unsigned short length);
void HandlePositionUpdate(GalaxySession *session, const unsigned char *data, unsigned short length);
void HandleSit(GalaxySession *session, const unsigned char *data, unsigned short length);
void HandleStand(GalaxySession *session, const unsigned char *data, unsigned short length);
void HandleSpatialChat(GalaxySession *session, const unsigned char *data, unsigned short length);
void HandleKneel(GalaxySession *session, const unsigned char *data, unsigned short length);
void HandleProne(GalaxySession *session, const unsigned char *data, unsigned short length);
void HandleMood(GalaxySession *session, const unsigned char *data, unsigned short length);
void HandleEmote(GalaxySession *session, const unsigned char *data, unsigned short length);

#endif // OPENSWG_SPATIAL_HANDLER_H
