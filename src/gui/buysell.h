/*
 *  The Mana World
 *  Copyright 2004 The Mana World Development Team
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
 *  $Id$
 */

#ifndef _TMW_BUYSELL_H
#define _TMW_BUYSELL_H

#include "window.h"

/**
 * A dialog to choose between buying or selling at a shop.
 *
 * \ingroup GUI
 */
class BuySellDialog : public Window, public gcn::ActionListener
{
    public:
        /**
         * Constructor. The action listener passed will receive "sell", "buy"
         * or "cancel" events when the respective buttons are pressed.
         *
         * @see Window::Window
         */
        BuySellDialog();

        /**
         * Destructor.
         */
        ~BuySellDialog();
       
        /**
         * Called when receiving actions from the widgets.
         */
        void action(const std::string& eventId);

    private:
        gcn::Button *buyButton;
        gcn::Button *sellButton;
        gcn::Button *cancelButton;
};

#endif
