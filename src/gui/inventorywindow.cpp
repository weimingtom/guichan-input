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
 */

#include "inventorywindow.h"

#include <string>

#include <guichan/mouseinput.hpp>
#include <guichan/widgets/label.hpp>

#include "button.h"
#include "gui.h"
#include "item_amount.h"
#include "itemcontainer.h"
#include "scrollarea.h"
#include "viewport.h"

#include "../inventory.h"
#include "../item.h"
#include "../localplayer.h"

#include "../resources/iteminfo.h"

#include "../utils/gettext.h"
#include "../utils/strprintf.h"

InventoryWindow::InventoryWindow():
    Window(_("Inventory"))
{
    setWindowName("Inventory");
    setResizable(true);
    setCloseButton(true);
    setMinWidth(240);
    setMinHeight(172);
    // If you adjust these defaults, don't forget to adjust the trade window's.
    setDefaultSize(115, 25, 322, 200);

    mUseButton = new Button(_("Use"), "use", this);
    mDropButton = new Button(_("Drop"), "drop", this);

    mItems = new ItemContainer(player_node->getInventory());
    mItems->addSelectionListener(this);

    mInvenScroll = new ScrollArea(mItems);
    mInvenScroll->setHorizontalScrollPolicy(gcn::ScrollArea::SHOW_NEVER);

    mItemNameLabel = new gcn::Label("Name:");
    mItemDescriptionLabel = new gcn::Label("Description:");
    mItemEffectLabel = new gcn::Label("Effect:");
    mWeightLabel = new gcn::Label("Weight:");
    mWeightLabel->setPosition(8, 8);
    mInvenScroll->setPosition(8,
            mWeightLabel->getY() + mWeightLabel->getHeight() + 5);
    mInvenSlotLabel = new gcn::Label("Slots used:");
    mInvenSlotLabel->setPosition(mWeightLabel->getX()
            + mWeightLabel->getWidth() + 100, 8);

    add(mUseButton);
    add(mDropButton);
    add(mInvenScroll);
    add(mItemNameLabel);
    add(mItemDescriptionLabel);
    add(mItemEffectLabel);
    add(mWeightLabel);
    add(mInvenSlotLabel);

    mUseButton->setSize(60, mUseButton->getHeight());

    loadWindowState();
}

void InventoryWindow::logic()
{
    Window::logic();

    // It would be nicer if this update could be event based, needs some
    // redesign of InventoryWindow and ItemContainer probably.
    updateButtons();

    // Update weight information
    mWeightLabel->setCaption(strprintf(_("Weight: %d / %d"),
                                       player_node->mTotalWeight,
                                       player_node->mMaxWeight));

    // Update number of items in inventory
    mInvenSlotLabel->setCaption(strprintf(_("Slots used: %d / %d"),
            player_node->getInventory()->getNumberOfSlotsUsed(),
            player_node->getInventory()->getInventorySize()));
}

void InventoryWindow::action(const gcn::ActionEvent &event)
{
    Item *item = mItems->getSelectedItem();

    if (!item)
        return;

    if (event.getId() == "use") {
        if (item->isEquipment()) {
            if (item->isEquipped()) {
                player_node->unequipItem(item);
            }
            else {
                player_node->equipItem(item);
            }
        }
        else {
            player_node->useItem(item);
        }
    }
    else if (event.getId() == "drop")
    {
        if (item->getQuantity() == 1) {
            player_node->dropItem(item, 1);
        }
        else {
            // Choose amount of items to drop
            new ItemAmountWindow(AMOUNT_ITEM_DROP, this, item);
        }
    }
}

void InventoryWindow::valueChanged(const gcn::SelectionEvent &event)
{
    const Item *item = mItems->getSelectedItem();

    // Update name, effect and description
    if (!item)
    {
        mItemNameLabel->setCaption(strprintf(_("Name: %s"), ""));
        mItemEffectLabel->setCaption(strprintf(_("Effect: %s"), ""));
        mItemDescriptionLabel->setCaption(strprintf(_("Description: %s"), ""));
    }
    else
    {
        const ItemInfo& itemInfo = item->getInfo();
        mItemNameLabel->setCaption(
                strprintf(_("Name: %s"), itemInfo.getName().c_str()));
        mItemEffectLabel->setCaption(
                strprintf(_("Effect: %s"), itemInfo.getEffect().c_str()));
        mItemDescriptionLabel->setCaption(
                strprintf(_("Description: %s"),
                          itemInfo.getDescription().c_str()));
    }
}

void InventoryWindow::mouseClicked(gcn::MouseEvent &event)
{
    Window::mouseClicked(event);

    if (event.getButton() == gcn::MouseEvent::RIGHT)
    {
        Item *item = mItems->getSelectedItem();

        if (!item)
            return;

        /* Convert relative to the window coordinates to absolute screen
         * coordinates.
         */
        const int mx = event.getX() + getX();
        const int my = event.getY() + getY();
        viewport->showPopup(mx, my, item);
    }
}

void InventoryWindow::widgetResized(const gcn::Event &event)
{
    Window::widgetResized(event);

    const gcn::Rectangle &area = getChildrenArea();
    const int width = area.width;
    const int height = area.height;

    // Adjust widgets
    mUseButton->setPosition(8, height - 8 - mUseButton->getHeight());
    mDropButton->setPosition(8 + mUseButton->getWidth() + 5,
            mUseButton->getY());

    mItemNameLabel->setDimension(gcn::Rectangle(8,
            mUseButton->getY() - 5 - mItemNameLabel->getHeight(),
            width - 16,
            mItemNameLabel->getHeight()));
    mItemEffectLabel->setDimension(gcn::Rectangle(8,
            mItemNameLabel->getY() - 5 - mItemEffectLabel->getHeight(),
            width - 16,
            mItemEffectLabel->getHeight()));
    mItemDescriptionLabel->setDimension(gcn::Rectangle(8,
            mItemEffectLabel->getY() - 5 - mItemDescriptionLabel->getHeight(),
            width - 16,
            mItemDescriptionLabel->getHeight()));

    mInvenScroll->setSize(width - 16,
            mItemDescriptionLabel->getY() - mWeightLabel->getHeight() - 18);

    mWeightLabel->setWidth(width - 16);
    mInvenSlotLabel->setWidth(width - 16);
}

void InventoryWindow::updateButtons()
{
    const Item *selectedItem = mItems->getSelectedItem();

    if (selectedItem && selectedItem->isEquipment())
    {
        if (selectedItem->isEquipped()) {
            mUseButton->setCaption(_("Unequip"));
        }
        else {
            mUseButton->setCaption(_("Equip"));
        }
    }
    else {
        mUseButton->setCaption(_("Use"));
    }

    mUseButton->setEnabled(selectedItem != 0);
    mDropButton->setEnabled(selectedItem != 0);
}

Item* InventoryWindow::getSelectedItem() const
{
    return mItems->getSelectedItem();
}
