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

#include "player.h"

#include "internal.h"

#include "net/manaserv/connection.h"
#include "net/manaserv/messageout.h"
#include "net/manaserv/protocol.h"

void RespawnRequestListener::action(const gcn::ActionEvent &event)
{
    ManaServ::GameServer::Player::respawn();
}

namespace ManaServ
{

void GameServer::Player::walk(int x, int y)
{
    MessageOut msg(PGMSG_WALK);
    msg.writeInt16(x);
    msg.writeInt16(y);
    GameServer::connection->send(msg);
}

void GameServer::Player::moveItem(int oldSlot, int newSlot, int amount)
{
    MessageOut msg(PGMSG_MOVE_ITEM);
    msg.writeInt8(oldSlot);
    msg.writeInt8(newSlot);
    msg.writeInt8(amount);
    GameServer::connection->send(msg);
}

void GameServer::Player::respawn()
{
    MessageOut msg(PGMSG_RESPAWN);
    GameServer::connection->send(msg);
}

}
