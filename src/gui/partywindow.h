/*
 *  The Mana World
 *  Copyright 2008 The Mana World Development Team
 *
 *  This file is part of The Mana World.
 *
 *  The Mana World is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  The Mana World is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Mana World; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  $Id: box.cpp 1456 2005-07-15 23:17:00Z b_lindeijer $
 */

#ifndef _TMW_PARTYWINDOW_H
#define _TMW_PARTYWINDOW_H

#include "window.h"

#include <string>
#include <vector>

class Player;

/**
 * Party Window.
 *
 * \ingroup Interface
 */
class PartyWindow : public Window
{
    public:
        /**
         * Constructor.
         */
        PartyWindow();

        /**
         * Draws the party window
         */
        void draw(gcn::Graphics *graphics);

        /**
         * Add party member
         */
        void addPartyMember(Player *player);

        /**
         * Remove party member
         */
        void removePartyMember(Player *player);

        /**
         * Show party invite
         */
        void showPartyInvite(const std::string &inviter);

    private:
        typedef std::vector<Player*> PartyList;
        PartyList mPartyMembers;
};

extern PartyWindow *partyWindow;

#endif
