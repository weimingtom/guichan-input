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

#ifndef CHAT_H
#define CHAT_H

#include <list>
#include <string>
#include <map>

#include <guichan/actionlistener.hpp>
#include <guichan/keylistener.hpp>
#include <guichan/widget.hpp>
#include <guichan/widgetlistener.hpp>

#include "widgets/chattab.h"

#include "window.h"

class BrowserBox;
class Channel;
class ChatTab;
class Recorder;
class ScrollArea;
class TabbedArea;
class ItemLinkHandler;
#ifdef EATHENA_SUPPORT
class Network;
#endif

enum
{
    BY_GM,
#ifdef EATHENA_SUPPORT
    BY_PARTY,
#endif
    BY_PLAYER,
    BY_OTHER,
    BY_SERVER,
    BY_CHANNEL,
    ACT_WHISPER,      // getting whispered at
    ACT_IS,           // equivalent to "/me" on IRC
    BY_LOGGER
};

/**
 * gets in between usernick and message text depending on
 * message type
 */
#define CAT_NORMAL        ": "
#define CAT_IS            ""
#define CAT_WHISPER       " whispers: "

#define DEFAULT_CHAT_WINDOW_SCROLL 7 // 1 means `1/8th of the window size'.

/** One item in the chat log */
struct CHATLOG
{
    std::string nick;
    std::string text;
    int own;
};

/**
 * The chat window.
 *
 * \ingroup Interface
 */
class ChatWindow : public Window,
                   public gcn::ActionListener,
                   public gcn::KeyListener
{
    public:
        /**
         * Constructor.
         */
#ifdef TMWSERV_SUPPORT
        ChatWindow();
#else
        ChatWindow(Network *network);
#endif

        /**
         * Destructor: used to write back values to the config file
         */
        ~ChatWindow();

        /**
         * Called when the widget changes size. Used for adapting the size of
         * the tabbed area.
         */
        void widgetResized(const gcn::Event &event);

        void logic();

        /**
         * Reset the chat window and recorder window attached to it to their
         * default positions.
         */
        void resetToDefaultSize();

        /**
         * Adds a line of text to our message list. Parameters:
         *
         * @param line Text message.
         * @param own  Type of message (usually the owner-type).
         * @param channelName which channel to send the message to.
         * @param ignoreRecord should this not be recorded?
         */
        void chatLog(std::string line,
                     int own = BY_SERVER,
                     std::string channelName = "",
                     bool ignoreRecord = false);

        /**
         * Gets the focused tab.
         */
        ChatTab* getFocused() const;

        /**
         * Clear the tab with the given name.
         */
        void clearTab(const std::string &tab);

        /**
         * Clear the given tab.
         */
        void clearTab(ChatTab* tab);

        /**
         * Clear the current tab.
         */
        void clearTab();

        /**
         * Performs action.
         */
        void action(const gcn::ActionEvent &event);

        /**
         * Request focus for typing chat message.
         *
         * \returns true if the input was shown
         *          false otherwise
         */
        bool requestChatFocus();

        /**
         * Checks whether ChatWindow is Focused or not.
         */
        bool isInputFocused();

        ChatTab* findTab(const std::string &tabName);

        /** Remove the given tab from the window */
        void removeTab(ChatTab *tab);

        void removeTab(const std::string &tabName);

        /** Add the tab to the window */
        void addTab(ChatTab *tab);

        /**
         * Passes the text to the current tab as input
         *
         * @param msg  The message text which is to be sent.
         *
         */
        void chatSend(std::string &msg);

        /** Called when key is pressed */
        void keyPressed(gcn::KeyEvent &event);

        /** Called to set current text */
        void setInputText(std::string input_str);

        /** Called to add item to chat */
        void addItemText(const std::string &item);

        /** Override to reset mTmpVisible */
        void setVisible(bool visible);

        /** Check if tab with that name already exists */
        bool tabExists(const std::string &tabName);

        /**
         * Scrolls the chat window
         *
         * @param amount direction and amount to scroll.  Negative numbers scroll
         * up, positive numbers scroll down.  The absolute amount indicates the
         * amount of 1/8ths of chat window real estate that should be scrolled.
         */
        void scroll(int amount);

#ifdef EATHENA_SUPPORT
        char getPartyPrefix() const { return mPartyPrefix; }
        void setPartyPrefix(char prefix) { mPartyPrefix = prefix; }
#endif

        /**
         * Sets the file being recorded to
         *
         * @param msg The file to write out to. If null, then stop recording.
         */
        void setRecordingFile(const std::string &msg);

        bool getReturnTogglesChat() const { return mReturnToggles; }
        void setReturnTogglesChat(bool toggles) { mReturnToggles = toggles; }

        void doPresent();

    protected:
        friend class ChatTab;

        void adjustTabSize();

#ifdef EATHENA_SUPPORT
        Network *mNetwork;
        char mPartyPrefix; /**< Messages beginning with the prefix are sent to
                              the party */
#endif
        /** Used for showing item popup on clicking links **/
        ItemLinkHandler *mItemLinkHandler;
        Recorder *mRecorder;

        /** Input box for typing chat messages. */
        gcn::TextField *mChatInput;

    private:
        bool mTmpVisible;

        /** Tabbed area for holding each channel. */
        TabbedArea *mChatTabs;

        typedef std::map<const std::string, ChatTab*> TabMap;
        /** Map each tab to its browser and scroll area. */
        TabMap mTabs;

        typedef std::list<std::string> History;
        typedef History::iterator HistoryIterator;
        History mHistory;           /**< Command history. */
        HistoryIterator mCurHist;   /**< History iterator. */
        bool mReturnToggles; /**< Marks whether <Return> toggles the chat log
                                or not */
};

extern ChatWindow *chatWindow;

#endif
