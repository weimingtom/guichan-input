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

#include <algorithm>

#include "browserbox.h"

#include "linkhandler.h"
#include "truetypefont.h"

BrowserBox::BrowserBox(unsigned int mode):
    gcn::Widget(),
    mMode(mode), mHighMode(UNDERLINE | BACKGROUND),
    mOpaque(true),
    mUseLinksAndUserColors(true),
    mSelectedLink(-1),
    mMaxRows(0)
{
    setFocusable(true);
    addMouseListener(this);
}

BrowserBox::~BrowserBox()
{
}

void BrowserBox::setLinkHandler(LinkHandler* linkHandler)
{
    mLinkHandler = linkHandler;
}

void BrowserBox::setOpaque(bool opaque)
{
    mOpaque = opaque;
}

void BrowserBox::setHighlightMode(unsigned int highMode)
{
    mHighMode = highMode;
}

void BrowserBox::disableLinksAndUserColors()
{
    mUseLinksAndUserColors = false;
}

void BrowserBox::addRow(const std::string &row)
{
    std::string tmp = row;
    std::string newRow;
    BROWSER_LINK bLink;
    int idx1, idx2, idx3;
    TrueTypeFont *font = static_cast<TrueTypeFont*>(getFont());

    // Use links and user defined colors
    if (mUseLinksAndUserColors)
    {
        // Check for links in format "@@link|Caption@@"
        idx1 = tmp.find("@@");
        while (idx1 >= 0)
        {
            idx2 = tmp.find("|", idx1);
            idx3 = tmp.find("@@", idx2);
            bLink.link = tmp.substr(idx1 + 2, idx2 - (idx1 + 2));
            bLink.caption = tmp.substr(idx2 + 1, idx3 - (idx2 + 1));
            bLink.y1 = mTextRows.size() * font->getHeight();
            bLink.y2 = bLink.y1 + font->getHeight();

            newRow += tmp.substr(0, idx1);

            std::string tmp2 = newRow;
            idx1 = tmp2.find("##");
            while (idx1 >= 0)
            {
                tmp2.erase(idx1, 3);
                idx1 = tmp2.find("##");
            }
            bLink.x1 = font->getWidth(tmp2) - 1;
            bLink.x2 = bLink.x1 + font->getWidth(bLink.caption) + 1;

            mLinks.push_back(bLink);

            newRow += "##L" + bLink.caption;

            tmp.erase(0, idx3 + 2);
            if(tmp != "")
            {
                newRow += "##P";
            }
            idx1 = tmp.find("@@");
        }

        newRow += tmp;
    }

    // Don't use links and user defined colors
    else
    {
        newRow = row;
    }

    mTextRows.push_back(newRow);

    //discard older rows when a row limit has been set
    if (mMaxRows > 0)
    {
        while (mTextRows.size() > mMaxRows) mTextRows.pop_front();
    }

    // Auto size mode
    if (mMode == AUTO_SIZE)
    {
        std::string plain = newRow;
        for (idx1 = plain.find("##"); idx1 >= 0; idx1 = plain.find("##"))
            plain.erase(idx1, 3);

        // Adjust the BrowserBox size
        int w = font->getWidth(plain);
        if (w > getWidth())
            setWidth(w);
    }

    if (mMode == AUTO_WRAP)
    {
        unsigned int y = 0;
        unsigned int nextChar;
        const char *hyphen = "~";
        int hyphenWidth = font->getWidth(hyphen);
        int x = 0;

        for (TextRowIterator i = mTextRows.begin(); i != mTextRows.end(); i++)
        {
            std::string row = *i;
            for (unsigned int j = 0; j < row.size(); j++)
            {
                std::string character = row.substr(j, 1);
                x += font->getWidth(character);
                nextChar = j + 1;

                // Wraping between words (at blank spaces)
                if ((nextChar < row.size()) && (row.at(nextChar) == ' '))
                {
                    int nextSpacePos = row.find(" ", (nextChar + 1));
                    if (nextSpacePos <= 0)
                    {
                        nextSpacePos = row.size() - 1;
                    }
                    int nextWordWidth = font->getWidth(
                            row.substr(nextChar,
                                (nextSpacePos - nextChar)));

                    if ((x + nextWordWidth + 10) > getWidth())
                    {
                        x = 15; // Ident in new line
                        y += 1;
                        j++;
                    }
                }
                // Wrapping looong lines (brutal force)
                else if ((x + 2 * hyphenWidth) > getWidth())
                {
                    x = 15; // Ident in new line
                    y += 1;
                }
            }
        }

        setHeight(font->getHeight() * (mTextRows.size() + y));
    }
    else
    {
        setHeight(font->getHeight() * mTextRows.size());
    }
}

void BrowserBox::clearRows()
{
    mTextRows.clear();
    mLinks.clear();
    setWidth(0);
    mSelectedLink = -1;
}

struct MouseOverLink
{
    MouseOverLink(int x, int y) : mX(x),mY(y) { }
    bool operator() (BROWSER_LINK &link)
    {
        return (mX >= link.x1 && mX < link.x2 &&
                mY >= link.y1 && mY < link.y2);
    }
    int mX, mY;
};

void
BrowserBox::mousePressed(gcn::MouseEvent &event)
{
    LinkIterator i = find_if(mLinks.begin(), mLinks.end(),
            MouseOverLink(event.getX(), event.getY()));

    if (i != mLinks.end()) {
        mLinkHandler->handleLink(i->link);
    }
}

void
BrowserBox::mouseMoved(gcn::MouseEvent &event)
{
    LinkIterator i = find_if(mLinks.begin(), mLinks.end(),
            MouseOverLink(event.getX(), event.getY()));

    mSelectedLink = (i != mLinks.end()) ? (i - mLinks.begin()) : -1;
}

void
BrowserBox::draw(gcn::Graphics *graphics)
{
    if (mOpaque)
    {
        graphics->setColor(gcn::Color(BGCOLOR));
        graphics->fillRectangle(gcn::Rectangle(0, 0, getWidth(), getHeight()));
    }

    if (mSelectedLink >= 0)
    {
        if ((mHighMode & BACKGROUND))
        {
            graphics->setColor(gcn::Color(HIGHLIGHT));
            graphics->fillRectangle(gcn::Rectangle(
                        mLinks[mSelectedLink].x1,
                        mLinks[mSelectedLink].y1,
                        mLinks[mSelectedLink].x2 - mLinks[mSelectedLink].x1,
                        mLinks[mSelectedLink].y2 - mLinks[mSelectedLink].y1
                        ));
        }

        if ((mHighMode & UNDERLINE))
        {
            graphics->setColor(gcn::Color(LINK));
            graphics->drawLine(
                    mLinks[mSelectedLink].x1,
                    mLinks[mSelectedLink].y2,
                    mLinks[mSelectedLink].x2,
                    mLinks[mSelectedLink].y2);
        }
    }

    int x = 0, y = 0;
    int wrappedLines = 0;
    TrueTypeFont *font = static_cast<TrueTypeFont*>(getFont());

    graphics->setColor(BLACK);
    for (TextRowIterator i = mTextRows.begin(); i != mTextRows.end(); i++)
    {
        int selColor = BLACK;
        int prevColor = selColor;
        std::string row = *(i);
        bool wrapped = false;
        x = 0;

        // Check for separator lines
        if (row.find("---", 0) == 0)
        {
            for (x = 0; x < getWidth(); x++)
            {
                font->drawString(graphics, "-", x, y);
                x += font->getWidth("-") - 2;
            }
            y += font->getHeight();
            continue;
        }

        // TODO: Check if we must take texture size limits into account here
        // TODO: Check if some of the O(n) calls can be removed
        for (std::string::size_type start = 0, end = std::string::npos;
                start != std::string::npos;
                start = end, end = std::string::npos)
        {
            // Wrapped line continuation shall be indented
            if (wrapped)
            {
                y += font->getHeight();
                x = 15;
            }

            // "Tokenize" the string at control sequences
            if (mUseLinksAndUserColors)
                end = row.find("##", start + 1);

            if (mUseLinksAndUserColors ||
                    (!mUseLinksAndUserColors && (start == 0)))
            {
                // Check for color change in format "##x", x = [L,P,0..9]
                if (row.find("##", start) == start && row.size() > start + 2)
                {
                    switch (row.at(start + 2))
                    {
                        case 'L': // Link color
                            prevColor = selColor;
                            selColor = LINK;
                            break;
                        case 'P': // Previous color
                            selColor = prevColor;
                            break;
                        case '1':
                            prevColor = selColor;
                            selColor = RED;
                            break;
                        case '2':
                            prevColor = selColor;
                            selColor = GREEN;
                            break;
                        case '3':
                            prevColor = selColor;
                            selColor = BLUE;
                            break;
                        case '4':
                            prevColor = selColor;
                            selColor = ORANGE;
                            break;
                        case '5':
                            prevColor = selColor;
                            selColor = YELLOW;
                            break;
                        case '6':
                            prevColor = selColor;
                            selColor = PINK;
                            break;
                        case '7':
                            prevColor = selColor;
                            selColor = PURPLE;
                            break;
                        case '8':
                            prevColor = selColor;
                            selColor = GRAY;
                            break;
                        case '9':
                            prevColor = selColor;
                            selColor = BROWN;
                            break;
                        case '0':
                        default:
                            prevColor = selColor;
                            selColor = BLACK;
                    }
                    start += 3;
                }
                graphics->setColor(gcn::Color(selColor));
            }

            std::string::size_type len =
                end == std::string::npos ? end : end - start;
            std::string part = row.substr(start, len);

            // Auto wrap mode
            if (mMode == AUTO_WRAP &&
                    (x + font->getWidth(part.c_str()) + 10) > getWidth())
            {
                bool forced = false;
                char const *hyphen = "~";
                int hyphenWidth =  font->getWidth(hyphen);

                do
                {
                    if (!forced)
                        end = row.rfind(" ", end);

                    // Check if we have to (stupidly) force-wrap
                    if (end == std::string::npos || end <= start)
                    {
                        forced = true;
                        end = row.size();
                        x += hyphenWidth * 2; // Account for the wrap-notifier
                        continue;
                    }

                    end--;

                    part = row.substr(start, end - start + 1);
                } while ((x + font->getWidth(part.c_str()) + 10) > getWidth());

                if (forced)
                {
                    x -= hyphenWidth; // Remove the wrap-notifier accounting
                    font->drawString(graphics, hyphen,
                            getWidth() - hyphenWidth, y);
                    end++; // Skip to the next character
                }
                else
                    end += 2; // Skip to after the space

                wrapped = true;
                wrappedLines++;
            }
            font->drawString(graphics, part, x, y);
            x += font->getWidth(part.c_str());
        }
        y += font->getHeight();
        setHeight((mTextRows.size() + wrappedLines) * font->getHeight());
    }
}
