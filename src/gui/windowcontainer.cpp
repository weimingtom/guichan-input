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

#include "windowcontainer.h"

WindowContainer::WindowContainer():
    gcn::Container(),
    mouseDown(false)
{
}

void WindowContainer::_mouseInputMessage(const gcn::MouseInput &mouseInput)
{
    if (mouseInput.getType() == gcn::MouseInput::PRESS)
    {
        mouseDown = true;
    }
    else if (mouseInput.getType() == gcn::MouseInput::RELEASE)
    {
        mouseDown = false;
    }

    /*
     * Make drag events not change widget with mouse. The Window instances
     * need this behaviour to be able to handle window dragging.
     */
    if (!(mouseInput.getType() == gcn::MouseInput::MOTION && mouseDown))
    {
        Widget* tempWidgetWithMouse = NULL;

        WidgetIterator iter;    
        for (iter = mWidgets.begin(); iter != mWidgets.end(); iter++)
        {
            if ((*iter)->getDimension().isPointInRect(
                        mouseInput.x, mouseInput.y) && (*iter)->isVisible())
            {
                tempWidgetWithMouse = (*iter);
            }
        }

        if (tempWidgetWithMouse != mWidgetWithMouse)
        {
            if (mWidgetWithMouse) {
                mWidgetWithMouse->_mouseOutMessage();
            }

            if (tempWidgetWithMouse) {
                tempWidgetWithMouse->_mouseInMessage();
            }

            mWidgetWithMouse = tempWidgetWithMouse;
        }
    }

    if (mWidgetWithMouse && !mWidgetWithMouse->hasFocus()) {
        gcn::MouseInput mi = mouseInput;
        mi.x -= mWidgetWithMouse->getX();
        mi.y -= mWidgetWithMouse->getY();      
        mWidgetWithMouse->_mouseInputMessage(mi);
    }

    if (mWidgetWithMouse == NULL) {
        gcn::Widget::_mouseInputMessage(mouseInput);
    }
}
