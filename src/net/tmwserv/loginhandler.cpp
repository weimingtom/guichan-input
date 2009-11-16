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

#include "net/tmwserv/loginhandler.h"

#include "net/tmwserv/connection.h"
#include "net/tmwserv/protocol.h"

#include "net/tmwserv/accountserver/account.h"
#include "net/tmwserv/accountserver/accountserver.h"

#include "net/logindata.h"
#include "net/messagein.h"

#include "main.h"

#include "utils/gettext.h"

Net::LoginHandler *loginHandler;

extern Net::Connection *accountServerConnection;

namespace TmwServ {

LoginHandler::LoginHandler()
{
    static const Uint16 _messages[] = {
        APMSG_LOGIN_RESPONSE,
        APMSG_REGISTER_RESPONSE,
        APMSG_RECONNECT_RESPONSE,
        APMSG_PASSWORD_CHANGE_RESPONSE,
        APMSG_EMAIL_CHANGE_RESPONSE,
        APMSG_LOGOUT_RESPONSE,
        APMSG_UNREGISTER_RESPONSE,
        0
    };
    handledMessages = _messages;
    loginHandler = this;
}

void LoginHandler::handleMessage(MessageIn &msg)
{
    switch (msg.getId())
    {
        case APMSG_LOGIN_RESPONSE:
            handleLoginResponse(msg);
            break;
        case APMSG_REGISTER_RESPONSE:
            handleRegisterResponse(msg);
            break;
        case APMSG_RECONNECT_RESPONSE:
        {
            int errMsg = msg.readInt8();
            // Successful login
            if (errMsg == ERRMSG_OK)
            {
                state = STATE_CHAR_SELECT;
            }
            // Login failed
            else
            {
                switch (errMsg) {
                    case ERRMSG_INVALID_ARGUMENT:
                        errorMessage = _("Wrong magic_token.");
                        break;
                    case ERRMSG_FAILURE:
                        errorMessage = _("Already logged in.");
                        break;
                    case LOGIN_SERVER_FULL:
                        errorMessage = _("Server is full.");
                        break;
                    default:
                        errorMessage = _("Unknown error.");
                        break;
                }
                state = STATE_ERROR;
            }
        }
            break;

        case APMSG_PASSWORD_CHANGE_RESPONSE:
        {
            int errMsg = msg.readInt8();
            // Successful pass change
            if (errMsg == ERRMSG_OK)
            {
                state = STATE_CHANGEPASSWORD_SUCCESS;
            }
            // pass change failed
            else
            {
                switch (errMsg) {
                    case ERRMSG_INVALID_ARGUMENT:
                        errorMessage = _("New password incorrect.");
                        break;
                    case ERRMSG_FAILURE:
                        errorMessage = _("Old password incorrect.");
                        break;
                    case ERRMSG_NO_LOGIN:
                        errorMessage = _("Account not connected. Please login first.");
                        break;
                    default:
                        errorMessage = _("Unknown error.");
                        break;
                }
                state = STATE_ACCOUNTCHANGE_ERROR;
            }
        }
            break;

        case APMSG_EMAIL_CHANGE_RESPONSE:
        {
            int errMsg = msg.readInt8();
            // Successful pass change
            if (errMsg == ERRMSG_OK)
            {
                state = STATE_CHANGEEMAIL_SUCCESS;
            }
            // pass change failed
            else
            {
                switch (errMsg) {
                    case ERRMSG_INVALID_ARGUMENT:
                        errorMessage = _("New email address incorrect.");
                        break;
                    case ERRMSG_FAILURE:
                        errorMessage = _("Old email address incorrect.");
                        break;
                    case ERRMSG_NO_LOGIN:
                        errorMessage = _("Account not connected. Please login first.");
                        break;
                    case ERRMSG_EMAIL_ALREADY_EXISTS:
                        errorMessage = _("The new email address already exists.");
                        break;
                    default:
                        errorMessage = _("Unknown error.");
                        break;
                }
                state = STATE_ACCOUNTCHANGE_ERROR;
            }
        }
            break;
        case APMSG_LOGOUT_RESPONSE:
        {
            int errMsg = msg.readInt8();

            // Successful logout
            if (errMsg == ERRMSG_OK)
            {
                // TODO: handle logout
            }
            // Logout failed
            else
            {
                switch (errMsg) {
                    case ERRMSG_NO_LOGIN:
                        errorMessage = "Accountserver: Not logged in";
                        break;
                    default:
                        errorMessage = "Accountserver: Unknown error";
                        break;
                }
                state = STATE_ERROR;
            }
        }
            break;
        case APMSG_UNREGISTER_RESPONSE:
        {
            int errMsg = msg.readInt8();
            // Successful unregistration
            if (errMsg == ERRMSG_OK)
            {
                state = STATE_UNREGISTER;
            }
            // Unregistration failed
            else
            {
                switch (errMsg) {
                    case ERRMSG_INVALID_ARGUMENT:
                        errorMessage =
                                  "Accountserver: Wrong username or password";
                        break;
                    default:
                        errorMessage = "Accountserver: Unknown error";
                        break;
                }
                state = STATE_ACCOUNTCHANGE_ERROR;
            }
        }
            break;
    }
}

void LoginHandler::handleLoginResponse(MessageIn &msg)
{
    const int errMsg = msg.readInt8();

    if (errMsg == ERRMSG_OK)
    {
        readUpdateHost(msg);
        // No worlds atm, but future use :-D
        state = STATE_WORLD_SELECT;
    }
    else
    {
        switch (errMsg) {
            case LOGIN_INVALID_VERSION:
                errorMessage = _("Client version is too old.");
                break;
            case ERRMSG_INVALID_ARGUMENT:
                errorMessage = _("Wrong username or password.");
                break;
            case ERRMSG_FAILURE:
                errorMessage = _("Already logged in.");
                break;
            case LOGIN_SERVER_FULL:
                errorMessage = _("Server is full.");
                break;
            case LOGIN_INVALID_TIME:
                errorMessage = _("Login attempt too soon after previous "
                                 "attempt.");
                break;
            default:
                errorMessage = _("Unknown error.");
                break;
        }
        state = STATE_LOGIN_ERROR;
    }
}

void LoginHandler::handleRegisterResponse(MessageIn &msg)
{
    const int errMsg = msg.readInt8();

    if (errMsg == ERRMSG_OK)
    {
        readUpdateHost(msg);
        state = STATE_WORLD_SELECT;
    }
    else
    {
        switch (errMsg) {
            case REGISTER_INVALID_VERSION:
                errorMessage = _("Client version is too old.");
                break;
            case ERRMSG_INVALID_ARGUMENT:
                errorMessage = _("Wrong username, password or email address.");
                break;
            case REGISTER_EXISTS_USERNAME:
                errorMessage = _("Username already exists.");
                break;
            case REGISTER_EXISTS_EMAIL:
                errorMessage = _("Email address already exists.");
                break;
            default:
                errorMessage = _("Unknown error.");
                break;
        }
        state = STATE_LOGIN_ERROR;
    }
}

void LoginHandler::readUpdateHost(MessageIn &msg)
{
    // Set the update host when included in the message
    if (msg.getUnreadLength() > 0)
    {
        mLoginData->updateHost = msg.readString();
    }
}

void LoginHandler::connect()
{
    accountServerConnection->connect(mServer.hostname, mServer.port);
    /*if (state == STATE_CONNECT_SERVER)
        state = STATE_LOGIN;*/
}

bool LoginHandler::isConnected()
{
    return accountServerConnection->isConnected();
}

void LoginHandler::disconnect()
{
    accountServerConnection->disconnect();
}

void LoginHandler::loginAccount(LoginData *loginData)
{
    mLoginData = loginData;
    Net::AccountServer::login(accountServerConnection,
            0,  // client version
            loginData->username,
            loginData->password);
}

void LoginHandler::logout()
{
    // TODO
}

void LoginHandler::changeEmail(const std::string &email)
{
    Net::AccountServer::Account::changeEmail(email);
}

void LoginHandler::changePassword(const std::string &username,
                    const std::string &oldPassword,
                    const std::string &newPassword)
{
    Net::AccountServer::Account::changePassword(username, oldPassword,
                                                newPassword);
}

void LoginHandler::chooseServer(unsigned int server)
{
    // TODO
}

void LoginHandler::registerAccount(LoginData *loginData)
{
    Net::AccountServer::registerAccount(accountServerConnection,
            0, // client version
            loginData->username,
            loginData->password,
            loginData->email);
}

void LoginHandler::unregisterAccount(const std::string &username,
                        const std::string &password)
{
    Net::AccountServer::Account::unregister(username, password);
}

Worlds LoginHandler::getWorlds() const
{
    return Worlds();
}

} // namespace TmwServ
