/*
 *  The Mana World
 *  Copyright (C) 2004  The Mana World Development Team
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

#define BOX_WIDTH 36
#define BOX_HEIGHT 36

#include <guichan/font.hpp>

#include "button.h"
#include "equipmentwindow.h"
#include "playerbox.h"

#include "../equipment.h"
#include "../graphics.h"
#include "../inventory.h"
#include "../item.h"
#include "../localplayer.h"

#include "../resources/iteminfo.h"
#include "../resources/resourcemanager.h"

#include "../utils/gettext.h"
#include "../utils/tostring.h"

// Positions of the boxes, 2nd dimension is X and Y respectively.
static const int boxPosition[][2] = {
    {50, 208},   // EQUIP_LEGS_SLOT
    {8, 123},    // EQUIP_FIGHT1_SLOT
    {8, 78},     // EQUIP_GLOVES_SLOT
    {129, 168},  // EQUIP_RING2_SLOT
    {8, 168},    // EQUIP_RING1_SLOT
    {129, 123},  // EQUIP_FIGHT2_SLOT
    {90, 208},   // EQUIP_FEET_SLOT
    {50, 40},    // EQUIP_CAPE_SLOT
    {70, 0},     // EQUIP_HEAD_SLOT
    {90, 40},    // EQUIP_TORSO_SLOT
    {129, 78}    // EQUIP_AMMO_SLOT
};

EquipmentWindow::EquipmentWindow(Equipment *equipment):
    Window(_("Equipment")),
    mEquipment(equipment),
    mSelected(-1)

{
    // Control that shows the Player
    mPlayerBox = new PlayerBox;
    mPlayerBox->setDimension(gcn::Rectangle(50, 80, 74, 123));
    mPlayerBox->setPlayer(player_node);

    setWindowName("Equipment");
    setCloseButton(true);
    setDefaultSize(5, 195, 180, 300);
    loadWindowState();

    mUnequip = new Button(_("Unequip"), "unequip", this);
    gcn::Rectangle const &area = getChildrenArea();
    mUnequip->setPosition(area.width  - mUnequip->getWidth() - 5,
                          area.height - mUnequip->getHeight() - 5);

    add(mPlayerBox);
    add(mUnequip);

    for (int i = EQUIP_LEGS_SLOT; i < EQUIP_VECTOREND; i++)
    {
        mEquipBox[i].posX = boxPosition[i][0] + getPadding();
        mEquipBox[i].posY = boxPosition[i][1] + getTitleBarHeight();
    }

    mInventory = player_node->getInventory();
}

EquipmentWindow::~EquipmentWindow()
{
    delete mUnequip;
}

void EquipmentWindow::draw(gcn::Graphics *graphics)
{
    // Draw window graphics
    Window::draw(graphics);

    Item* item;

    Graphics *g = static_cast<Graphics*>(graphics);

    Window::drawChildren(graphics);

    for (int i = EQUIP_LEGS_SLOT; i < EQUIP_VECTOREND; i++)
    {
        item = (i != EQUIP_AMMO_SLOT) ?
               mInventory->getItem(mEquipment->getEquipment(i)) :
               mInventory->getItem(mEquipment->getArrows());
        if (item)
        {
            // Draw Item.
            Image* image = item->getImage();
            g->drawImage(image, mEquipBox[i].posX, mEquipBox[i].posY);
            if (i == EQUIP_AMMO_SLOT)
            {
                g->setColor(gcn::Color(0, 0, 0));
                graphics->drawText(toString(item->getQuantity()),
                                   mEquipBox[i].posX + (BOX_WIDTH / 2),
                                   mEquipBox[i].posY - getFont()->getHeight(),
                                   gcn::Graphics::CENTER); 
            }
        }

        if (i == mSelected)
        {
            // Set color red.
            g->setColor(gcn::Color(255, 0, 0));
        }
        else
        {
            // Set color black.
            g->setColor(gcn::Color(0, 0, 0));
        }

        // Draw box border.
        g->drawRectangle(gcn::Rectangle(mEquipBox[i].posX, mEquipBox[i].posY,
            BOX_WIDTH, BOX_HEIGHT));
    }
}

void EquipmentWindow::action(const gcn::ActionEvent &event)
{
    if (event.getId() == "unequip" && mSelected > -1)
    {
        Item* item = (mSelected != EQUIP_AMMO_SLOT) ?
                     mInventory->getItem(mEquipment->getEquipment(mSelected)) :
                     mInventory->getItem(mEquipment->getArrows());
        player_node->unequipItem(item);
        mSelected = -1;
    }
}

void EquipmentWindow::mousePressed(gcn::MouseEvent& mouseEvent)
{
    Window::mousePressed(mouseEvent);

    const int x = mouseEvent.getX();
    const int y = mouseEvent.getY();

    Item* item;

    // Checks if any of the presses were in the equip boxes.
    for (int i = EQUIP_LEGS_SLOT; i < EQUIP_VECTOREND; i++)
    {
        item = (i != EQUIP_AMMO_SLOT) ?
               mInventory->getItem(mEquipment->getEquipment(i)) :
               mInventory->getItem(mEquipment->getArrows());
        gcn::Rectangle tRect(mEquipBox[i].posX, mEquipBox[i].posY,
                             BOX_WIDTH, BOX_HEIGHT);
        if (tRect.isPointInRect(x, y))
        {
            if (item)
            {
                mSelected = i;
            }
        }
    }
}

