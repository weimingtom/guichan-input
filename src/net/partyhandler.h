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

#ifndef PARTYHANDLER_H
#define PARTYHANDLER_H

#include <string>

namespace Net {

class PartyHandler
{
    public:
        virtual void create(const std::string &name = "") = 0;

        virtual void join(int partyId) = 0;

        virtual void invite(int playerId) = 0;

        virtual void inviteResponse(bool accept) = 0;

        virtual void leave() = 0;

        virtual void kick(int playerId) = 0;

        virtual void chat(const std::string &text) = 0;

        virtual void requestPartyMembers() = 0;

        // virtual void options() = 0;

        // virtual void message() = 0;
};

} // namespace Net

#endif // PARTYHANDLER_H
