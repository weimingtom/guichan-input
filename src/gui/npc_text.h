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
 *  $Id: npc_text.h 4096 2008-04-16 08:39:59Z b_lindeijer $
 */

#ifndef _TMW_NPC_TEXT_H
#define _TMW_NPC_TEXT_H

#include <iosfwd>
#include <guichan/actionlistener.hpp>

#include "scrollarea.h"
#include "button.h"
#include "window.h"

#include "../guichanfwd.h"

class TextBox;

/**
 * The npc text dialog.
 *
 * \ingroup Interface
 */
class NpcTextDialog : public Window, public gcn::ActionListener
{
    public:
        /**
         * Constructor.
         *
         * @see Window::Window
         */
        NpcTextDialog();

        /**
         * Called when resizing the window
         *
         * @param event The calling event
         */
        void widgetResized(const gcn::Event &event);

        /**
         * Redraws the window
         */
        void draw();

        /**
         * Called when receiving actions from the widgets.
         */
        void
        action(const gcn::ActionEvent &event);

        /**
         * Sets the text shows in the dialog.
         *
         * @param string The new text.
         */
        void
        setText(const std::string &string);

        /**
         * Adds the text to the text shows in the dialog. Also adds a newline
         * to the end.
         *
         * @param string The text to add.
         */
        void
        addText(const std::string &string);

    private:
        gcn::Button *okButton;
        gcn::ScrollArea *scrollArea;
        TextBox *mTextBox;

        std::string mText;
};

#endif
