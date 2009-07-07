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

#include "gui/skilldialog.h"

#include "gui/widgets/button.h"
#include "gui/widgets/container.h"
#include "gui/widgets/icon.h"
#include "gui/widgets/label.h"
#include "gui/widgets/layouthelper.h"
#include "gui/widgets/listbox.h"
#include "gui/widgets/progressbar.h"
#include "gui/widgets/scrollarea.h"
#include "gui/widgets/tab.h"
#include "gui/widgets/tabbedarea.h"
#include "gui/widgets/vertcontainer.h"
#include "gui/widgets/windowcontainer.h"

#include "localplayer.h"
#include "log.h"

#include "utils/dtor.h"
#include "utils/gettext.h"
#include "utils/stringutils.h"
#include "utils/xml.h"

#include <string>
#include <vector>

class SkillEntry;

struct SkillInfo
{
    unsigned short id;
    std::string name;
    std::string icon;
    bool modifiable;
    SkillEntry *display;
};

class SkillEntry : public Container
{
    public:
        SkillEntry(struct SkillInfo *info);

        void update();

    private:
        struct SkillInfo *mInfo;
        Icon *mIcon;
        Label *mNameLabel;
        Label *mLevelLabel;
        Label *mExpLabel;
        ProgressBar *mProgress;
};

SkillDialog::SkillDialog():
    Window(_("Skills"))
{
    setWindowName("Skills");
    setCloseButton(true);
    setResizable(true);
    setSaveVisible(true);
    setDefaultSize(windowContainer->getWidth() - 280, 30, 275, 425);

    mTabs = new TabbedArea();

    place(0, 0, mTabs, 5, 5);
    place(0, 5, new Label("TODO"));

    setLocationRelativeTo(getParent());
    loadWindowState();
}

SkillDialog::~SkillDialog()
{
    //delete_all(mTabs);
}

void SkillDialog::action(const gcn::ActionEvent &event)
{
    if (event.getId() == "skill")
    {
    }
    else if (event.getId() == "close")
    {
        setVisible(false);
    }
}

void SkillDialog::adjustTabSize()
{
    gcn::Widget *content = mTabs->getCurrentWidget();
    if (content) {
        int width = mTabs->getWidth() - 2 * content->getFrameSize();
        int height = mTabs->getContainerHeight() - 2 * content->getFrameSize();
        content->setSize(width, height);
        content->setVisible(true);
        content->logic();
    }
}

void SkillDialog::widgetResized(const gcn::Event &event)
{
    Window::widgetResized(event);

    adjustTabSize();
}

void SkillDialog::logic()
{
    Window::logic();

    Tab *tab = dynamic_cast<Tab*>(mTabs->getSelectedTab());
    if (tab != mCurrentTab) {
        mCurrentTab = tab;
        adjustTabSize();
    }
}

std::string SkillDialog::update(int id)
{
    SkillInfo *info = mSkills[id];

    if (info)
    {
        info->display->update();
        return info->name;
    }
    else
        return "";
}

void SkillDialog::loadSkills(const std::string &file, bool fixed)
{
    // TODO: mTabs->clear();
    delete_all(mSkills);

    XML::Document doc(file);
    xmlNodePtr root = doc.rootNode();

    if (!root || !xmlStrEqual(root->name, BAD_CAST "skills"))
    {
        logger->log("Error loading skills file: %s", file.c_str());
        return;
    }

    int setCount = 0;
    std::string setName;
    ScrollArea *scroll;
    VertContainer *container;
    std::string fixedDef = toString(fixed);

    for_each_xml_child_node(set, root)
    {
        if (xmlStrEqual(set->name, BAD_CAST "set"))
        {
            setCount++;
            setName = XML::getProperty(set, "name", strprintf(_("Skill Set %d"), setCount));

            container = new VertContainer(32);
            container->setOpaque(false);
            scroll = new ScrollArea(container);
            scroll->setOpaque(false);
            scroll->setHorizontalScrollPolicy(ScrollArea::SHOW_NEVER);
            scroll->setVerticalScrollPolicy(ScrollArea::SHOW_ALWAYS);

            mTabs->addTab(setName, scroll);
            for_each_xml_child_node(node, set)
            {
                if (xmlStrEqual(node->name, BAD_CAST "skill"))
                {
                    int id = atoi(XML::getProperty(node, "id", "-1").c_str());
                    std::string name = XML::getProperty(node, "name", strprintf(_("Skill %d"), id));
                    std::string icon = XML::getProperty(node, "icon", "");
                    bool modifiable = !atoi(XML::getProperty(node, "fixed", fixedDef).c_str());

                    SkillInfo *skill = new SkillInfo;
                    skill->id = id;
                    skill->name = name;
                    skill->icon = icon;
                    skill->modifiable = modifiable;
                    skill->display = new SkillEntry(skill);

                    container->add(skill->display);

                    mSkills[id] = skill;
                }
            }
        }
    }

    adjustTabSize();
}

SkillEntry::SkillEntry(struct SkillInfo *info) : mInfo(info),
    mIcon(NULL),
    mNameLabel(new Label(info->name)),
    mProgress(new ProgressBar(0.0f, 200, 20, gcn::Color(150, 150, 150))),
    mLevelLabel(new Label("999"))
{
    setOpaque(false);

    if (!info->icon.empty())
        mIcon = new Icon(info->icon);

    /*LayoutHelper h(this);
    ContainerPlacer place = h.getPlacer(0, 0);

    if (mIcon)
        place(0, 0, mIcon, 1, 2);
    place(1, 0, mNameLabel, 3);
    place(4, 0, mLevelLabel);
    place(1, 1, mProgress, 4);*/

    if (mIcon)
    {
        mIcon->setPosition(1, 0);
        add(mIcon);
    }

    mNameLabel->setPosition(35, 0);
    add(mNameLabel);

    mLevelLabel->setPosition(165, 0);
    add(mLevelLabel);

    mProgress->setPosition(35, 13);
    add(mProgress);

    update();
}

void SkillEntry::update()
{
    int baseLevel = player_node->getAttributeBase(mInfo->id);

    int effLevel = player_node->getAttributeEffective(mInfo->id);

    if (baseLevel <= 0)
    {
        setVisible(false);
        return;
    }

    setVisible(true);

    std::string skillLevel("Lvl: " + toString(baseLevel));
    if (effLevel < baseLevel)
    {
        skillLevel.append(" - " + toString(baseLevel - effLevel));
    }
    else if (effLevel > baseLevel)
    {
        skillLevel.append(" + " + toString(effLevel - baseLevel));
    }
    mLevelLabel->setCaption(skillLevel);

    std::pair<int, int> exp = player_node->getExperience(mInfo->id);
    std::string sExp (toString(exp.first) + " / " + toString(exp.second));

    mLevelLabel->adjustSize();
    mProgress->setText(sExp);

    // More intense red as exp grows
    int color = 150 - (int)(150 * ((float) exp.first / exp.second));
    mProgress->setColor(244, color, color);
    mProgress->setProgress((float) exp.first / exp.second);
}
