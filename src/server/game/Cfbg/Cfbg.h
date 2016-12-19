/*
* Copyright (C) 2016 RustEmuCore <http://rustemu.org/>
* Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CUSTOM_H
#define _CUSTOM_H

#define MSG_COLOR_LIGHTRED     "|cffff6060"
#define MSG_COLOR_LIGHTBLUE    "|cff00ccff"
#define MSG_COLOR_ANN_GREEN    "|c1f40af20"
#define MSG_COLOR_RED          "|cffff0000"
#define MSG_COLOR_GOLD         "|cffffcc00"
#define MSG_COLOR_SUBWHITE     "|cffbbbbbb"
#define MSG_COLOR_MAGENTA      "|cffff00ff"
#define MSG_COLOR_YELLOW       "|cffffff00"
#define MSG_COLOR_CYAN         "|cff00ffff"
#define MSG_COLOR_DARKBLUE     "|cff0000ff"

#define MSG_COLOR_GREY         "|cff9d9d9d"
#define MSG_COLOR_WHITE        "|cffffffff"
#define MSG_COLOR_GREEN        "|cff1eff00"
#define MSG_COLOR_BLUE         "|cff0080ff"
#define MSG_COLOR_PURPLE       "|cffb048f8"
#define MSG_COLOR_ORANGE       "|cffff8000"

#define MSG_COLOR_DRUID        "|cffff7d0a"
#define MSG_COLOR_HUNTER       "|cffabd473"
#define MSG_COLOR_MAGE         "|cff69ccf0"
#define MSG_COLOR_PALADIN      "|cfff58cba"
#define MSG_COLOR_PRIEST       "|cffffffff"
#define MSG_COLOR_ROGUE        "|cfffff569"
#define MSG_COLOR_SHAMAN       "|cff0070de"
#define MSG_COLOR_WARLOCK      "|cff9482c9"
#define MSG_COLOR_WARRIOR      "|cffc79c6e"
#define MSG_COLOR_DEATH_KNIGHT "|cffc41f3b"
#define MSG_COLOR_MONK         "|cff00ff96"

#define LIMIT_UINT32 2147483647

enum FakeMorphs
{
    FAKE_F_TAUREN = 20584,
    FAKE_M_TAUREN = 20585,

    FAKE_M_NELF = 20318,
    FAKE_F_NELF = 2230,

    FAKE_F_DRAENEI = 20323,
    FAKE_M_DRAENEI = 20323,

    FAKE_F_ORC = 1312,
    FAKE_M_ORC = 19182,

    FAKE_F_TROLL = 4084,
    FAKE_M_TROLL = 4083,

    FAKE_F_BLOODELF = 20579,
    FAKE_M_BLOODELF = 20578,

    FAKE_F_UNDEAD = 2674,
    FAKE_M_UNDEAD = 19188,

    FAKE_F_HUMAN = 19724,
    FAKE_M_HUMAN = 19723,

    FAKE_F_GNOME = 9131,
    FAKE_M_GNOME = 3117,

    FAKE_F_DWARF = 6588,
    FAKE_M_DWARF = 3078
};
#endif
