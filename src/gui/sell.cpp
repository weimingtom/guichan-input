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

#include "sell.h"
#include "button.h"
#include "slider.h"
#include "scrollarea.h"
#include "../graphic/graphic.h"

#include <sstream>

SellDialog::SellDialog(gcn::Container *parent):
    Window(parent, "Sell")
{
    itemList = new gcn::ListBox(this);
    scrollArea = new ScrollArea(itemList);
    slider = new Slider(1.0);
    quantityLabel = new gcn::Label("0");
    okButton = new Button("OK");
    cancelButton = new Button("Cancel");

    setSize(260, 175);
    scrollArea->setDimension(gcn::Rectangle(5, 5, 250, 110));
    slider->setDimension(gcn::Rectangle(5, 120, 200, 10));
    quantityLabel->setPosition(215, 120);
    okButton->setPosition(180, 145);
    cancelButton->setPosition(208, 145);

    itemList->setEventId("item");
    slider->setEventId("slider");
    okButton->setEventId("ok");
    cancelButton->setEventId("cancel");

    itemList->addActionListener(this);
    slider->addActionListener(this);
    okButton->addActionListener(this);
    cancelButton->addActionListener(this);

    add(scrollArea);
    add(slider);
    add(quantityLabel);
    add(okButton);
    add(cancelButton);

    setLocationRelativeTo(getParent());
}

SellDialog::~SellDialog()
{
    delete cancelButton;
    delete okButton;
    delete slider;
    delete itemList;
    delete scrollArea;
}

void SellDialog::reset()
{
    shopInventory.clear();
}

void SellDialog::addItem(short index, int price)
{
    int id = inventoryWindow->items[index].id;
    ITEM_SHOP item_shop;

    if (id >= 501 && id <= 511) {
        sprintf(item_shop.name, "%s %i gp", item_db[id - 501], price);
    }
    else {
        sprintf(item_shop.name, "Unknown item %i gp", price);
    }
    item_shop.price = price;
    item_shop.index = index;
    item_shop.id = id;
    item_shop.quantity = inventoryWindow->items[index].quantity;

    shopInventory.push_back(item_shop);
    itemList->adjustSize();
}

void SellDialog::action(const std::string& eventId)
{
    int selectedItem = itemList->getSelected();

    if (eventId == "slider" || eventId == "item") {
        if (selectedItem > -1) {
            int maxItems = shopInventory[selectedItem].quantity;
            std::stringstream ss;

            ss << (int)(slider->getValue() * maxItems);
            quantityLabel->setCaption(ss.str());
            quantityLabel->adjustSize();
        }
    }
    else if (eventId == "ok") {
        if (selectedItem > -1) {
            // Attempt sell
            int maxItems = shopInventory[selectedItem].quantity;
            int amount = (int)(slider->getValue() * maxItems);

            if (amount > 0) {
                WFIFOW(0) = net_w_value(0x00c9);
                WFIFOW(2) = net_w_value(8);
                WFIFOW(4) = net_w_value(shopInventory[selectedItem].index);
                WFIFOW(6) = net_w_value(amount);
                WFIFOSET(8);
            }
        }
        setVisible(false);
    }
    else if (eventId == "cancel") {
        setVisible(false);
    }
}

int SellDialog::getNumberOfElements()
{
    return shopInventory.size();
}

std::string SellDialog::getElementAt(int i)
{
    return shopInventory[i].name;
}
