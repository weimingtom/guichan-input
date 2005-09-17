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

#include "messageout.h"

#include <string>
//#include <SDL_net.h>
#include "win2mac.h"

#include "network.h"
#include "packet.h"

MessageOut::MessageOut():
    mPacket(0),
    mData(0),
    mDataSize(0),
    mPos(0)
{
    // TODO: data not to be already allocated, keep it this way unitl full
    // conversion
    mData = out + out_size;
}

MessageOut::~MessageOut()
{
    if (mPacket) {
        delete mPacket;
    }

    // Don't free this data for now, see above.
    //if (mData) {
    //    free(mData);
    //}
}

void MessageOut::expand(size_t bytes)
{
    /*mData = (char*)realloc(mData, bytes);
    mDataSize = bytes;*/
}

void MessageOut::writeByte(char value)
{
    expand(mPos + sizeof(char));
    mData[mPos] = value;
    mPos += sizeof(char);
}

void MessageOut::writeShort(short value)
{
    expand(mPos + sizeof(short));
#ifdef SDL_BYTEORDER == SDL_BIG_ENDIAN
    (*(short *)(mData + mPos)) = DR_SwapTwoBytes(value);
#else
    (*(short *)(mData + mPos)) = value;
#endif
    //SDLNet_Write16(value, &mData[mPos]);
    mPos += sizeof(short);
}

void MessageOut::writeLong(long value)
{
    expand(mPos + sizeof(long));
#ifdef SDL_BYTEORDER == SDL_BIG_ENDIAN
    (*(long *)(mData + mPos)) = DR_SwapFourBytes(value);
#else
    (*(long *)(mData + mPos)) = value;
#endif
    //SDLNet_Write32(value, &mData[mPos]);
    mPos += sizeof(long);
}

void MessageOut::writeString(const std::string &string, int length)
{
    std::string toWrite = string;

    if (length < 0)
    {
        // Write the length at the start if not fixed
        writeShort(string.length());
        expand(mPos + string.length());
    }
    else
    {
        // Make sure the length of the string is no longer than specified
        toWrite = string.substr(0, length);
        expand(mPos + length);
    }
    
    // Write the actual string
    memcpy(&mData[mPos], (void*)toWrite.c_str(), toWrite.length());
    mPos += toWrite.length();

    // Pad remaining space with zeros
    if (length > (int)toWrite.length())
    {
        memset(&mData[mPos], '\0', length - toWrite.length());
        mPos += length - toWrite.length();
    }
}

const Packet *MessageOut::getPacket()
{
    if (!mPacket)
    {
        mPacket = new Packet(mData, mDataSize);
    }

    return mPacket;
}

MessageOut& operator<<(MessageOut &msg, const char &rhs)
{
    msg.writeByte(rhs);
    return msg;
}

MessageOut& operator<<(MessageOut &msg, const short &rhs)
{
    msg.writeShort(rhs);
    return msg;
}

MessageOut& operator<<(MessageOut &msg, const long &rhs)
{
    msg.writeLong(rhs);
    return msg;
}
