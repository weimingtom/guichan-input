/*
 *  Custom keyboard shortcuts configuration
 *  Copyright (C) 2007  Joshua Langley <joshlangley@optusnet.com.au>
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

#ifndef GUI_SETUP_KEYBOARD_H
#define GUI_SETUP_KEYBOARD_H

#include "guichanfwd.h"
#include "keyboardconfig.h"

#include "gui/setuptab.h"

#include <guichan/actionlistener.hpp>
#include <guichan/keylistener.hpp>

#include <string>

class Setup_Keyboard : public SetupTab, public gcn::ActionListener,
                       public gcn::KeyListener
{
    public:
        /**
         * Constructor
         */
        Setup_Keyboard();

        /**
         * Destructor
         */
        ~Setup_Keyboard();

        void apply();
        void cancel();

        void action(const gcn::ActionEvent &event);

        /**
         * Get an update on the assigned key.
         */
        void refreshAssignedKey(KeyboardConfig::KeyAction index);

        /**
         * Shorthand method to update all the keys.
         */
        void refreshKeys();

        void keyPressed(gcn::KeyEvent &event);

        void keyReleased(gcn::KeyEvent &event);

    private:
        class KeyListModel *mKeyListModel;
        gcn::ListBox *mKeyList;

        gcn::Button *mAssignKeyButton;
        gcn::Button *mClearKeyButton;
        gcn::Button *mMakeDefaultButton;

        gcn::Label *mCurrentKeyLabel;
        KeyData mCurrentKey;
        bool mKeySetting; /**< flag to check if key being set. */
};

#endif
