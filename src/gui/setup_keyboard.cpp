/*
 *  Custom keyboard shortcuts configuration
 *  Copyright (C) 2007  Joshua Langley <joshlangley@optusnet.com.au>
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

#include "gui/setup_keyboard.h"

#include "gui/gui.h"
#include "gui/okdialog.h"
#include "gui/setup.h"

#include "gui/widgets/button.h"
#include "gui/widgets/label.h"
#include "gui/widgets/layouthelper.h"
#include "gui/widgets/listbox.h"
#include "gui/widgets/scrollarea.h"

#include "utils/gettext.h"
#include "utils/stringutils.h"

#include <guichan/listmodel.hpp>

/**
 * The list model for key function list.
 *
 * \ingroup Interface
 */
class KeyListModel : public gcn::ListModel
{
    public:
        /**
         * Returns the number of elements in container.
         */
        int getNumberOfElements() { return keyboard.KEY_TOTAL; }

        /**
         * Returns element from container.
         */
        std::string getElementAt(int i) { return mKeyFunctions[i]; }

        /**
         * Sets element from container.
         */
        void setElementAt(int i, const std::string &caption)
        {
            mKeyFunctions[i] = caption;
        }

    private:
        std::string mKeyFunctions[KeyboardConfig::KEY_TOTAL];
};

Setup_Keyboard::Setup_Keyboard():
    mKeyListModel(new KeyListModel),
    mKeyList(new ListBox(mKeyListModel)),
    mKeySetting(false)
{
    setName(_("Keyboard"));
    setFocusable(true);
    addKeyListener(this);

    refreshKeys();

    mKeyList->addActionListener(this);

    ScrollArea *scrollArea = new ScrollArea(mKeyList);
    scrollArea->setHorizontalScrollPolicy(gcn::ScrollArea::SHOW_NEVER);

    mAssignKeyButton = new Button(_("Assign"), "assign", this);
    mAssignKeyButton->addActionListener(this);
    mAssignKeyButton->setEnabled(false);

    mClearKeyButton = new Button(_("Clear"), "clear", this);
    mClearKeyButton->addActionListener(this);
    mClearKeyButton->setEnabled(false);

    mMakeDefaultButton = new Button(_("Default"), "makeDefault", this);
    mMakeDefaultButton->addActionListener(this);

    mCurrentKeyLabel = new Label();

    // Do the layout
    LayoutHelper h(this);
    ContainerPlacer place = h.getPlacer(0, 0);

    place(0, 0, scrollArea, 5, 6).setPadding(2);
    place(0, 6, mMakeDefaultButton);
    place(1, 6, mCurrentKeyLabel, 2);
    place(3, 6, mClearKeyButton);
    place(4, 6, mAssignKeyButton);

    setDimension(gcn::Rectangle(0, 0, 365, 280));
}

Setup_Keyboard::~Setup_Keyboard()
{
    delete mKeyList;
    delete mKeyListModel;

    delete mAssignKeyButton;
    delete mMakeDefaultButton;
}

void Setup_Keyboard::apply()
{
    if (keyboard.hasConflicts())
    {
        new OkDialog(_("Key Conflict(s) Detected."),
                     _("Resolve them, or gameplay may result in strange "
                       "behaviour."));
    }
    keyboard.store();
}

void Setup_Keyboard::cancel()
{
    keyboard.retrieve();

    refreshKeys();
}

void Setup_Keyboard::action(const gcn::ActionEvent &event)
{
    int i = mKeyList->getSelected();
    if (event.getSource() == mKeyList)
    {
        if (!mKeySetting)
        {
            mAssignKeyButton->setEnabled(true);
        }
        if (!keyboard.keyMatch(i, KeyboardConfig::NULL_KEY))
        {
            mClearKeyButton->setEnabled(true);
        }
    }
    else if (event.getId() == "assign")
    {
        mKeySetting = true;
        requestFocus();
        mAssignKeyButton->setEnabled(false);
        mClearKeyButton->setEnabled(false);
        mKeyListModel->setElementAt(i, keyboard.getKeyCaption(i) + ": ?");
    }
    else if (event.getId() == "clear")
    {
        keyboard.setKeyData(i, KeyboardConfig::NULL_KEY);
        refreshAssignedKey((KeyboardConfig::KeyAction) i);
        mClearKeyButton->setEnabled(false);
    }
    else if (event.getId() == "makeDefault")
    {
        keyboard.resetToDefaults();
        refreshKeys();
    }
}

void Setup_Keyboard::refreshAssignedKey(KeyboardConfig::KeyAction index)
{
    if (index < 0)
        return;
    std::string caption = keyboard.getKeyCaption(index) + ": " + keyboard.keyString(index);
    mKeyListModel->setElementAt(index, caption);
}

void Setup_Keyboard::refreshKeys()
{
    for (int i = 0; i < keyboard.KEY_TOTAL; i++)
    {
        refreshAssignedKey((KeyboardConfig::KeyAction) i);
    }
}

void Setup_Keyboard::keyPressed(gcn::KeyEvent &event)
{
    if (mKeySetting)
    {
        mCurrentKey = keyboard.keyConvert(event);
        mCurrentKeyLabel->setCaption(keyboard.keyString(mCurrentKey));
        event.consume();
    }
}

void Setup_Keyboard::keyReleased(gcn::KeyEvent &event)
{
    if (mKeySetting)
    {
        mKeySetting = false;
        int i = mKeyList->getSelected();
        keyboard.setKeyData(i, mCurrentKey);
        mCurrentKey = KeyboardConfig::NULL_KEY;
        mCurrentKeyLabel->setCaption("");
        refreshAssignedKey((KeyboardConfig::KeyAction) i);
        mAssignKeyButton->setEnabled(true);
        event.consume();
    }
}
