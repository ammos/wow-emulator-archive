/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ChannelMgr.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1( AllianceChannelMgr );
INSTANTIATE_SINGLETON_1( HordeChannelMgr );

void WorldSession::HandleChannelJoin(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    uint32 unk1;
    uint8 unk2;
    std::string channelname, pass;

    recvPacket >> unk1 >> unk2;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> pass;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        cMgr->GetJoinChannel(channelname)->Join(_player->GetGUID(),pass.c_str(), unk1);
}

void WorldSession::HandleChannelLeave(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1);

    uint32 unk;
    std::string channelname;
    recvPacket >> unk;
    recvPacket >> channelname;

    if(!channelname.length())
        return;

    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
    {
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->Leave(_player->GetGUID(),true,0);
        cMgr->LeftChannel(channelname);
    }
}

void WorldSession::HandleChannelList(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1);

    std::string channelname;
    recvPacket >> channelname;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->List(_player->GetGUID());
}

void WorldSession::HandleChannelPassword(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    std::string channelname, pass;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> pass;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->Password(_player->GetGUID(),pass.c_str());
}

void WorldSession::HandleChannelSetOwner(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    std::string channelname, newp;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> newp;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->SetOwner(_player->GetGUID(),newp.c_str());
}

void WorldSession::HandleChannelOwner(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1);

    std::string channelname;
    recvPacket >> channelname;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->GetOwner(_player->GetGUID());
}

void WorldSession::HandleChannelModerator(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    std::string channelname, otp;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> otp;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->SetModerator(_player->GetGUID(),otp.c_str());
}

void WorldSession::HandleChannelUnmoderator(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    std::string channelname, otp;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> otp;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->UnsetModerator(_player->GetGUID(),otp.c_str());
}

void WorldSession::HandleChannelMute(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    std::string channelname, otp;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> otp;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->SetMute(_player->GetGUID(),otp.c_str());
}

void WorldSession::HandleChannelUnmute(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    std::string channelname, otp;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> otp;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->UnsetMute(_player->GetGUID(),otp.c_str());
}

void WorldSession::HandleChannelInvite(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    std::string channelname, otp;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> otp;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->Invite(_player->GetGUID(),otp.c_str());
}

void WorldSession::HandleChannelKick(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    std::string channelname, otp;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> otp;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->Kick(_player->GetGUID(),otp.c_str());
}

void WorldSession::HandleChannelBan(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    std::string channelname, otp;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> otp;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->Ban(_player->GetGUID(),otp.c_str());
}

void WorldSession::HandleChannelUnban(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1+1);

    std::string channelname, otp;
    recvPacket >> channelname;

    // recheck
    CHECK_PACKET_SIZE(recvPacket,(channelname.size()+1)+1);

    recvPacket >> otp;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->UnBan(_player->GetGUID(),otp.c_str());
}

void WorldSession::HandleChannelAnnounce(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1);

    std::string channelname;
    recvPacket >> channelname;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->Announce(_player->GetGUID());
}

void WorldSession::HandleChannelModerate(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket,1);

    std::string channelname;
    recvPacket >> channelname;
    if(ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if(Channel *chn = cMgr->GetChannel(channelname,_player))
            chn->Moderate(_player->GetGUID());
}
