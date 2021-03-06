/*
 *  The Mana World
 *  Copyright (C) 2009  The Mana World Development Team
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

#ifndef GUILDHANDLER_H
#define GUILDHANDLER_H

#include "guild.h"
#include "player.h"

#include <iosfwd>

namespace Net {

class GuildHandler
{
    public:
        virtual void create(const std::string &name) = 0;

        virtual void invite(int guildId, const std::string &name) = 0;

        virtual void invite(int guildId, Player *player) = 0;

        virtual void inviteResponse(int guildId, bool response) = 0;

        virtual void leave(int guildId) = 0;

        virtual void kick(GuildMember member) = 0;

        virtual void chat(int guildId, const std::string &text) = 0;

        virtual void memberList(int guildId) = 0;

        virtual void changeMemberPostion(GuildMember member, int level) = 0;

        virtual void requestAlliance(int guildId, int otherGuildId) = 0;

        virtual void requestAllianceResponse(int guildId, int otherGuildId,
                                             bool response) = 0;

        virtual void endAlliance(int guildId, int otherGuildId) = 0;
};

}

#endif // GUILDHANDLER_H
