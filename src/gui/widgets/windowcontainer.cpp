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

#include "gui/widgets/windowcontainer.h"

#include "../gui.h"

#include "gui/widgets/window.h"

#include "utils/dtor.h"

WindowContainer *windowContainer = NULL;

void WindowContainer::logic()
{
    delete_all(mDeathList);
    mDeathList.clear();

    gcn::Container::logic();
}

void WindowContainer::scheduleDelete(gcn::Widget *widget)
{
    mDeathList.push_back(widget);
}

void WindowContainer::nextWindow()
{
    Window *win = gui->getFocusedWindow();

    if (!win)
    {
        mWidgets.front()->requestFocus();
        return;
    }

    WidgetListIterator i = mWidgets.begin();

    for (; i != mWidgets.end(); i++)
    {
        Widget *w = (*i);
        if (w == win)
            break;
    }

    i++;

    if (i == mWidgets.end())
        i = mWidgets.begin();

    for (; i != mWidgets.end(); i++)
    {
        Widget *w = (*i);
        if (w->isFocusable())
        {
            (*i)->requestFocus();
        }
    }
}

void WindowContainer::previousWindow()
{
    Window *win = gui->getFocusedWindow();

    if (!win)
    {
        mWidgets.back()->requestFocus();
        return;
    }

    WidgetListIterator i = mWidgets.end();

    for (; i != mWidgets.begin(); i--)
    {
        Widget *w = (*i);
        if (w == win)
            break;
    }

    i--;

    if (i == mWidgets.begin())
        i = mWidgets.end();

    for (; i != mWidgets.begin(); i++)
    {
        Widget *w = (*i);
        if (w && w->isFocusable())
        {
            (*i)->requestFocus();
        }
    }
}

