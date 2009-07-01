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

#ifndef OPENSWG_SOE_HANDLER_H
#define OPENSWG_SOE_HANDLER_H

#include <tr1/memory>

class Session;
class ByteBuffer;

void HandleSessionRequest(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleNetStatus(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleMultiPacket(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleAcknowledge(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleDataChannel(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandleDisconnect(Session& session, std::tr1::shared_ptr<ByteBuffer> message);
void HandlePing(Session& session, std::tr1::shared_ptr<ByteBuffer> message);

#endif // OPENSWG_SOE_HANDLER_H