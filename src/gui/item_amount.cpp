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

#include "item_amount.h"

#include "button.h"
#include "inttextbox.h"
#include "slider.h"
#include "trade.h"

#include "../item.h"
#include "../localplayer.h"

#include "../utils/gettext.h"

ItemAmountWindow::ItemAmountWindow(int usage, Window *parent, Item *item,
    int maxRange):
    Window("", true, parent),
    mItem(item)
{
    // New labels
    mItemAmountTextBox = new IntTextBox(1);

    // New buttons
    Button *minusButton = new Button("-", "Minus", this);
    Button *plusButton = new Button("+", "Plus", this);
    Button *okButton = new Button(_("Ok"), "Drop", this);
    Button *cancelButton = new Button(_("Cancel"), "Cancel", this);
    if (!maxRange) {
        maxRange = mItem->getQuantity();
    }
    mItemAmountSlide = new Slider(1.0, maxRange);

    mItemAmountTextBox->setRange(1, maxRange);
    mItemAmountSlide->setDimension(gcn::Rectangle(5, 120, 180, 10));

    // Set button events Id
    mItemAmountSlide->setActionEventId("Slide");

    // Set position
    mItemAmountTextBox->setPosition(35, 10);
    mItemAmountTextBox->setSize(24, 16);
    plusButton->setPosition(60, 5);
    minusButton->setPosition(10, 5);
    mItemAmountSlide->setPosition(10, 35);
    okButton->setPosition(10, 50);
    cancelButton->setPosition(60, 50);

    // Assemble
    add(mItemAmountTextBox);
    add(plusButton);
    add(minusButton);
    add(mItemAmountSlide);
    add(okButton);
    add(cancelButton);

    mItemAmountSlide->addActionListener(this);

    resetAmount();

    switch (usage) {
        case AMOUNT_TRADE_ADD:
            setCaption(_("Select amount of items to trade."));
            okButton->setActionEventId("AddTrade");
            break;
        case AMOUNT_ITEM_DROP:
            setCaption(_("Select amount of items to drop."));
            okButton->setActionEventId("Drop");
            break;
        case AMOUNT_ITEM_SPLIT:
            setCaption(_("Select amount of items to split."));
            okButton->setActionEventId("Split");
            break;
        default:
            break;
    }

    setContentSize(200, 80);
    setLocationRelativeTo(getParentWindow());
    setVisible(true);
}

void ItemAmountWindow::resetAmount()
{
    mItemAmountTextBox->setInt(1);
}

void ItemAmountWindow::action(const gcn::ActionEvent &event)
{
    int amount = mItemAmountTextBox->getInt();

    if (event.getId() == "Cancel")
    {
        scheduleDelete();
    }
    else if (event.getId() == "Plus")
    {
        amount++;
    }
    else if (event.getId() == "Minus")
    {
        amount--;
    }
    else if (event.getId() == "Slide")
    {
        amount = static_cast<int>(mItemAmountSlide->getValue());
    }
    else if (event.getId() == "Drop")
    {
        player_node->dropItem(mItem, mItemAmountTextBox->getInt());
        scheduleDelete();
    }
    else if (event.getId() == "AddTrade")
    {
        tradeWindow->tradeItem(mItem, mItemAmountTextBox->getInt());
        scheduleDelete();
    }
    else if (event.getId() == "Split")
    {
        player_node->splitItem(mItem, mItemAmountTextBox->getInt());
        scheduleDelete();
    }
    mItemAmountTextBox->setInt(amount);
    mItemAmountSlide->setValue(amount);
}
