/*
 *  The Mana World
 *  Copyright (C) 2004  The Mana World Development Team
 *
 *  This file is part of The Mana World.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "gameserver.h"

#include "internal.h"

#include "net/manaserv/connection.h"
#include "net/manaserv/messageout.h"
#include "net/manaserv/protocol.h"

namespace ManaServ
{

void GameServer::connect(Connection *connection, const std::string &token)
{
    GameServer::connection = connection;

    MessageOut msg(PGMSG_CONNECT);

    msg.writeString(token, 32);

    GameServer::connection->send(msg);
}

void GameServer::logout(bool reconnectAccount)
{
    MessageOut msg(PGMSG_DISCONNECT);

    msg.writeInt8((unsigned char) reconnectAccount);

    GameServer::connection->send(msg);
}

}
