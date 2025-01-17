// Copyright (C) 2004 WoW Daemon
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Log.h"
#include "Opcodes.h"
#include "ObjectMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "UpdateMask.h"
#include "Player.h"
#include "Quest.h"
#include "Spell.h"
#include "UpdateData.h"
#include "Channel.h"

Player::Player ( ): Unit()
{
    m_objectType |= TYPE_PLAYER;
    m_objectTypeId = TYPEID_PLAYER;

    m_valuesCount = PLAYER_END;

    m_session = 0;

    m_afk = 0;
    m_curTarget = 0;
    m_curSelection = 0;
    m_lootGuid = 0;
    m_guildId = 0;
    m_petInfoId = 0;
    m_petLevel = 0;
    m_petFamilyId = 0;

    m_regenTimer = 0;
    m_dismountTimer = 0;
    m_dismountCost = 0;
    m_mount_pos_x = 0;
    m_mount_pos_y = 0;
    m_mount_pos_z = 0;

    m_currentSpell = NULL;

    memset(m_items, 0, sizeof(Item*)*INVENTORY_SLOT_ITEM_END);

    m_groupLeader = 0;
    m_isInGroup = false;
    m_isInvited = false;
}


Player::~Player ( )
{
    ASSERT(!IsInWorld());

    for(int i = 0; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        if(m_items[i])
            delete m_items[i];
    }
	CleanupChannels();
}

///====================================================================
///  Create
///  params: p_newChar
///  desc:   data from client to create a new character
//====================================================================
void Player::Create( uint32 guidlow, WorldPacket& data )
{
    int i;
    uint8 race,class_,gender,skin,face,hairStyle,hairColor,facialHair,outfitId;
    uint32 baseattacktime[2];

    Object::_Create(guidlow, HIGHGUID_PLAYER);

    for (i = 0; i < INVENTORY_SLOT_ITEM_END; i++)
        m_items[i] = NULL;

    // unpack data into member variables
    data >> m_name;
    data >> race >> class_ >> gender >> skin >> face;
    data >> hairStyle >> hairColor >> facialHair >> outfitId;

    //////////  Constant for everyone  ////////////////
    // Starting Locs
    // Human(1): 0, -8949.95, -132.493, 83.5312
    // Orc(2): 1, -618.518, -4251.67, 38.718
    // Dwarf(3): 0, -6240.32, 331.033, 382.758
    // Night Elf(4): 1, 10311.3, 832.463, 1326.41
    // Undead(5): 0, 1676.35, 1677.45, 121.67
    // Tauren(6): 1, -2917.58, -257.98, 52.9968
    // Gnome(7): See Dwarf
    // Troll(8): See Orc

    // LEFT SIDE
    // Head        0
    // Neck        1
    // Shoulders   2
    // Back        14
    // Chest       4
    // Shirt       3
    // Tabard      18
    // Wrists      8

    // RIGHT SIDE
    // Hands       9
    // Waist       5
    // Legs        6
    // Feet        7
    // Finger A    10
    // Finger B    11
    // Trinket A   12
    // Trinket B   13

    // WIELDED
    // Main hand   15
    // Offhand     16
    // Ranged      17

    PlayerCreateInfo *info = objmgr.GetPlayerCreateInfo(race, class_);
    ASSERT(info);

    baseattacktime[0] = 2000;
    baseattacktime[1] = 2000;

    m_mapId = info->mapId;
    m_zoneId = info->zoneId;
    m_positionX = info->positionX;
    m_positionY = info->positionY;
    m_positionZ = info->positionZ;
    memset(m_taximask, 0, sizeof(m_taximask));

    uint8 powertype = 0;
    switch(class_)
    {
    case WARRIOR : powertype = 1; break; // Rage
    case PALADIN : powertype = 0; break; // Mana
    case HUNTER  : powertype = 0; break;
    case ROGUE   : powertype = 3; break; // Energy
    case PRIEST  : powertype = 0; break;
    case SHAMAN  : powertype = 0; break;
    case MAGE    : powertype = 0; break;
    case WARLOCK : powertype = 0; break;
    case DRUID   : powertype = 0; break;
    } // 2 = Focus (unused)

    // Set Starting stats for char
    SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
    SetUInt32Value(UNIT_FIELD_HEALTH, info->health);
    SetUInt32Value(UNIT_FIELD_POWER1, info->mana );
    SetUInt32Value(UNIT_FIELD_POWER2, 0 ); // this gets devided by 10
    SetUInt32Value(UNIT_FIELD_POWER3, info->focus );
    SetUInt32Value(UNIT_FIELD_POWER4, info->energy );
    //SetUInt32Value(UNIT_FIELD_POWER5, 0xEEEEEEEE );
    SetUInt32Value(UNIT_FIELD_MAXHEALTH, info->health);
    SetUInt32Value(UNIT_FIELD_MAXPOWER1, info->mana );
    SetUInt32Value(UNIT_FIELD_MAXPOWER2, info->rage );
    SetUInt32Value(UNIT_FIELD_MAXPOWER3, info->focus );
    SetUInt32Value(UNIT_FIELD_MAXPOWER4, info->energy );
    //SetUInt32Value(UNIT_FIELD_MAXPOWER5, 5 );
    SetUInt32Value(UNIT_FIELD_LEVEL, 1 );
    SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 1 );
    SetUInt32Value(UNIT_FIELD_BYTES_0, ( ( race ) | ( class_ << 8 ) | ( gender << 16 ) | ( powertype << 24 ) ) );
    SetUInt32Value(UNIT_FIELD_BYTES_1, 0x0011EE00 );
    SetUInt32Value(UNIT_FIELD_BYTES_2, 0xEEEEEE00 );
    SetUInt32Value(UNIT_FIELD_FLAGS , 0x08 );
    SetUInt32Value(UNIT_FIELD_STAT0, info->strength );
    SetUInt32Value(UNIT_FIELD_STAT1, info->ability );
    SetUInt32Value(UNIT_FIELD_STAT2, info->stamina );
    SetUInt32Value(UNIT_FIELD_STAT3, info->intellect );
    SetUInt32Value(UNIT_FIELD_STAT4, info->spirit );
    SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, baseattacktime[0] );
    SetUInt32Value(UNIT_FIELD_BASEATTACKTIME+1, baseattacktime[1]  );
    SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.388999998569489f );
    SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f   );
    SetUInt32Value(UNIT_FIELD_DISPLAYID, info->displayId + gender );
    SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, info->displayId + gender );
    SetFloatValue(UNIT_FIELD_MINDAMAGE, info->mindmg );
    SetFloatValue(UNIT_FIELD_MAXDAMAGE, info->maxdmg );
    SetUInt32Value(UNIT_FIELD_ATTACKPOWER, info->attackpower );
    SetUInt32Value(PLAYER_BYTES, ((skin) | (face << 8) | (hairStyle << 16) | (hairColor << 24)));
    SetUInt32Value(PLAYER_BYTES_2, (facialHair | (0xEE << 8) | (0x01 << 16) | (0x01 << 24)));
    SetUInt32Value(PLAYER_NEXT_LEVEL_XP, 800);
    SetUInt32Value(PLAYER_FIELD_BYTES, 0xEEEE0000 );

    Item *item;
    for (i=0; i<10; i++)
    {
        if ( (info->item[i]!=0) && (info->item_slot[i]!=0) )
        {
            item = new Item();
            item->Create(objmgr.GenerateLowGuid(HIGHGUID_ITEM), info->item[i], this);
            AddItemToSlot(info->item_slot[i], item);
        }
    }

    for (i=0; i<10; i++)
    {
        if ( info->spell[i]!=0 )
        {
            addSpell(info->spell[i], 0);
        }
    }

    // Not worrying about this stuff for now
    m_guildId = 0;
    m_petInfoId = 0;
    m_petLevel = 0;
    m_petFamilyId = 0;
}


void Player::Update( uint32 p_time )
{
    if(!IsInWorld())
        return;

    Unit::Update( p_time );

    if(m_regenTimer > 0)
    {
        if(p_time >= m_regenTimer)
            m_regenTimer = 0;
        else
            m_regenTimer -= p_time;
    }

    if (m_state & UF_ATTACKING)
    {
        // In combat!
        if (isAttackReady())
        {
            Unit *pVictim = NULL;

            pVictim = objmgr.GetObject<Creature>(m_curSelection);

            if (!pVictim)
            {
                Log::getSingleton( ).outDetail("Player::Update:  No valid current selection to attack, stopping attack\n");
                clearStateFlag(UF_ATTACKING);
                smsg_AttackStop(m_curSelection);
            }
            else
            {
                if(canReachWithAttack(pVictim))
                {
                    AttackerStateUpdate(pVictim,0,false);
                    setAttackTimer();
                }
            }
        }
    }

    // only regenerate if NOT in combat, and if alive
    else if (isAlive())
    {
        // Regenerate health, mana and energy if necessary.
        Regenerate( UNIT_FIELD_HEALTH, UNIT_FIELD_MAXHEALTH, true );
        Regenerate( UNIT_FIELD_POWER1, UNIT_FIELD_MAXPOWER1, true );
        Regenerate( UNIT_FIELD_POWER2, UNIT_FIELD_MAXPOWER2, false );
        Regenerate( UNIT_FIELD_POWER4, UNIT_FIELD_MAXPOWER4, true );
    }

    // Dead System
    if (m_deathState == JUST_DIED)
    {
        KillPlayer();
    }

    // Auto-Dismount after Taxiride
    if(m_dismountTimer > 0)
    {
        if(p_time >= m_dismountTimer)
        {
            m_dismountTimer = 0;

            SetUInt32Value( PLAYER_FIELD_COINAGE , m_dismountCost);
            m_dismountCost = 0;

            SetPosition( m_mount_pos_x,
                m_mount_pos_y,
                m_mount_pos_z, true );

            SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID , 0);
            RemoveFlag( UNIT_FIELD_FLAGS ,0x000004 );
            RemoveFlag( UNIT_FIELD_FLAGS, 0x002000 );

            SetPlayerSpeed(RUN,7.5f,false);
           
        }
        else
            m_dismountTimer -= p_time;
    }

    /*
    //This was replaced by teleporting to spirit healer.
    if (m_timeOfDeath > 0 && (uint32)time(NULL) > m_timeOfDeath + m_corpseDelay){
    m_timeOfDeath = 0;
    m_respawnDelay = 5000;
    session->GetPlayer( )->SetUInt32Value( PLAYER_BYTES_2, (0xffffffff - 0x10) & session->GetPlayer( )->GetUInt32Value( PLAYER_BYTES_2 ) );
    session->GetPlayer( )->SetUInt32Value( UNIT_FIELD_FLAGS, (0xffffffff - 65536) & session->GetPlayer( )->GetUInt32Value( UNIT_FIELD_FLAGS ) );
    session->GetPlayer()->setDeathState(ALIVE);
    }
    */

    //UpdateObject();
}

/// Regenerates the regenField's curValue to the maxValue
/// Right now, everything regenerates at the same rate
/// A possible mod is to add another parameter, the stat regeneration is based off of (Intelligence for mana, Strength for HP)
/// And build a regen rate based on that
void Player::Regenerate(uint16 field_cur, uint16 field_max, bool switch_)
{
    uint32 curValue = GetUInt32Value(field_cur);
    uint32 maxValue = GetUInt32Value(field_max);

    if (switch_)
    {
        if (curValue >= maxValue)
            return;
    }
    else
    {
        if (curValue == 0)
            return;
    }

    uint32 regenDelay = 2000;

    // check if it's time to regen health
    if (m_regenTimer != 0)
        return;

    m_regenTimer = regenDelay;

    switch (field_cur)
    {
    case UNIT_FIELD_HEALTH:
        curValue+=uint32((GetUInt32Value(UNIT_FIELD_STAT4) * .15));  //FIXME
        break;
    case UNIT_FIELD_POWER1:
        curValue+=uint32((GetUInt32Value(UNIT_FIELD_STAT4) * .15));  //FIXME
        break;
    case UNIT_FIELD_POWER2:
        curValue-=uint32((GetUInt32Value(UNIT_FIELD_STAT4) * 1.5));  //FIXME
        break;
    case UNIT_FIELD_POWER4:
        curValue+=uint32((GetUInt32Value(UNIT_FIELD_STAT4) * .15));  //FIXME
        break;
    }

    if (switch_)
    {
        if (curValue > maxValue) curValue = maxValue;
        SetUInt32Value(field_cur, curValue);
    }
    else
    {
        if (curValue > maxValue) curValue = 0;
        SetUInt32Value(field_cur, curValue);
    }
}

void Player::BuildEnumData( WorldPacket * p_data )
{
    *p_data << GetGUID();
    *p_data << m_name;

    uint32 bytes = GetUInt32Value(UNIT_FIELD_BYTES_0);
    *p_data << uint8(bytes & 0xff); // race
    *p_data << uint8((bytes >> 8) & 0xff); // class
    *p_data << uint8((bytes >> 16) & 0xff); // gender

    bytes = GetUInt32Value(PLAYER_BYTES);
    *p_data << uint8(bytes & 0xff); //skin
    *p_data << uint8((bytes >> 8) & 0xff); //face
    *p_data << uint8((bytes >> 16) & 0xff); //hairstyle
    *p_data << uint8((bytes >> 24) & 0xff); //haircolor

    bytes = GetUInt32Value(PLAYER_BYTES_2);
    *p_data << uint8(bytes & 0xff); //facialhair

    *p_data << uint8(GetUInt32Value(UNIT_FIELD_LEVEL)); //level

    *p_data << GetZoneId();
    *p_data << GetMapId();

    *p_data << m_positionX;
    *p_data << m_positionY;
    *p_data << m_positionZ;

    *p_data << (uint32)0;             // guild
    *p_data << (uint32)0;             // unknown
    *p_data << (uint8)1;              // rest state
    *p_data << (uint32)m_petInfoId;   // pet info id
    *p_data << (uint32)m_petLevel;    // pet level
    *p_data << (uint32)m_petFamilyId; // pet family id


    for (int i = 0; i < 20; i++)
    {
        if (m_items[i] != NULL)
        {
            *p_data << (uint32)m_items[i]->GetProto()->DisplayInfoID;
            *p_data << (uint8)m_items[i]->GetProto()->InventoryType;
        }
        else
        {
            *p_data << (uint32)0;
            *p_data << (uint8)0;
        }
    }

    //    assert( p_data->getLength() <= 176 );
}



/////////////////////////////////// QUESTS ////////////////////////////////////////////
uint32 Player::getQuestStatus(uint32 quest_id)
{
    if( mQuestStatus.find( quest_id ) == mQuestStatus.end( ) ) return 0;
    return mQuestStatus[quest_id].status;
}

uint32 Player::addNewQuest(uint32 quest_id, uint32 status)
{
    quest_status qs;
    qs.quest_id = quest_id;
    qs.status = status;

    mQuestStatus[quest_id] = qs;
    return status;
};

void Player::loadExistingQuest(quest_status qs)
{
    mQuestStatus[qs.quest_id] = qs;
}

void Player::setQuestStatus(uint32 quest_id, uint32 new_status)
{
    assert( mQuestStatus.find( quest_id ) != mQuestStatus.end( ) );
    mQuestStatus[quest_id].status = new_status;
}

uint16 Player::getOpenQuestSlot()
{
    uint16 start = PLAYER_QUEST_LOG_1_1;
    uint16 end = PLAYER_QUEST_LOG_1_1 + 80;
    for (uint16 i = start; i <= end; i+=4)
        if (GetUInt32Value(i) == 0)
            return i;

    return 0;
}

uint16 Player::getQuestSlot(uint32 quest_id)
{
    uint16 start = PLAYER_QUEST_LOG_1_1;
    uint16 end = PLAYER_QUEST_LOG_1_1 + 80;
    for (uint16 i = start; i <= end; i+=4)
        if (GetUInt32Value(i) == quest_id)
            return i;

    return 0;
}

/*
void Player::setQuestLogBits(UpdateMask *updateMask)
{
for( StatusMap::iterator i = mQuestStatus.begin( ); i != mQuestStatus.end( ); ++ i ) {
if (i->second.status == 3)
{
// incomplete, put the quest in the log
uint16 log_slot = getQuestSlot(i->second.quest_id);
struct quest_status qs = i->second;

if (log_slot == 0){ // in case this quest hasnt been added to the updateValues (but it shoudl have been!)
log_slot = getOpenQuestSlot();
SetUInt32Value(log_slot, qs.quest_id);
SetUInt32Value(log_slot+1, 0x337);
}

updateMask->setBit(log_slot);
updateMask->setBit(log_slot+1);

if (qs.m_questMobCount[0] > 0 || qs.m_questMobCount[1] > 0 ||
qs.m_questMobCount[2] > 0 || qs.m_questMobCount[3] > 0)
{
updateMask->setBit(log_slot+2);
}
}
}
}*/


void Player::KilledMonster(uint32 entry, const uint64 &guid)
{
    for( StatusMap::iterator i = mQuestStatus.begin( ); i != mQuestStatus.end( ); ++ i )
    {
        if (i->second.status == 3)
        {
            Quest *pQuest = objmgr.GetQuest(i->first);
            for (int j=0; j<4; j++)
            {
                if (pQuest->m_questMobId[j] == entry)
                {
                    if (i->second.m_questMobCount[j]+1 <= pQuest->m_questMobCount[j])
                    {
                        i->second.m_questMobCount[j]++ ;

                        // Send Update quest update kills message
                        WorldPacket data;
                        data.Initialize(SMSG_QUESTUPDATE_ADD_KILL);
                        data << pQuest->m_questId;
                        data << uint32(pQuest->m_questMobId[j]);
                        data << uint32(i->second.m_questMobCount[j]);
                        data << uint32(pQuest->m_questMobCount[j]);
                        data << guid;
                        GetSession()->SendPacket(&data);

                        // update journal
                        // this is crazy.  each bit corresponds to a kill, set multiple bits to signify multiple kills
                        uint32 start_bit=0;
                        if (j-1 < 0) start_bit = 0;
                        else
                            for (int n=j-1; n>=0; n--)
                                start_bit += pQuest->m_questMobCount[n];

                        uint16 log_slot = getQuestSlot(pQuest->m_questId);
                        uint32 kills = GetUInt32Value(log_slot+2);

                        int exp = start_bit + i->second.m_questMobCount[j]-1;
                        kills |= 1 << exp;
                        SetUInt32Value(log_slot+2, kills);
                    }

                    checkQuestStatus(i->second.quest_id);
                    // Ehh, I think a packet should be sent here, but I havent found one in the official logs yet

                    return;
                } // end if mobId == entry
            } // end for each mobId
        } // end if status == 3
    } // end for each quest
}

//======================================================
///  Check to see if all the required monsters and items
///  have been killed and collected.
//======================================================
bool Player::checkQuestStatus(uint32 quest_id)
{
    assert( mQuestStatus.find( quest_id ) != mQuestStatus.end( ) );
    quest_status qs = mQuestStatus[quest_id];
    Quest *pQuest = objmgr.GetQuest(quest_id);

    if (qs.m_questItemCount[0] == pQuest->m_questItemCount[0] &&
        qs.m_questItemCount[1] == pQuest->m_questItemCount[1] &&
        qs.m_questItemCount[2] == pQuest->m_questItemCount[2] &&
        qs.m_questItemCount[3] == pQuest->m_questItemCount[3] &&
        qs.m_questMobCount[0] == pQuest->m_questMobCount[0] &&
        qs.m_questMobCount[1] == pQuest->m_questMobCount[1] &&
        qs.m_questMobCount[2] == pQuest->m_questMobCount[2] &&
        qs.m_questMobCount[3] == pQuest->m_questMobCount[3])
    {
        // Quest complete!
        return true;
    }

    return false;
}


///  This function sends the message displaying the purple XP gain for the char
///  It assumes you will send out an UpdateObject packet at a later time.
void Player::GiveXP(uint32 xp, const uint64 &guid)
{
    if (xp == 0)
        return;

    WorldPacket data;
    if (guid != 0)
    {
        // Send out purple XP gain message, but ONLY if a valid GUID was passed in
        // This message appear to be only for gaining XP from a death
        data.Initialize( SMSG_LOG_XPGAIN );
        data << guid;
        data << uint32(xp) << uint8(0);
        data << uint16(xp) << uint8(0);
        data << uint8(0);
        GetSession()->SendPacket(&data);
    }

    uint32 curXP = GetUInt32Value(PLAYER_XP);
    uint32 nextLvlXP = GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
    uint32 newXP = curXP + xp;

    // Check for level-up
    if (newXP >= nextLvlXP)
    {
        uint32 healthGain = 0, newHealth = 0, manaGain = 0, newMana=0;
        // Level-Up!
        newXP = newXP - nextLvlXP;  // reset XP to 0, but add extra from this xp add
        nextLvlXP += nextLvlXP/2;   // set the new next level xp

        uint16 level = (uint16)GetUInt32Value(UNIT_FIELD_LEVEL) + 1;    // increment the level

        healthGain = GetUInt32Value(UNIT_FIELD_STAT2) / 2;
        newHealth = GetUInt32Value(UNIT_FIELD_MAXHEALTH) + healthGain;

        if (GetUInt32Value(UNIT_FIELD_POWER1) > 0)
        {
            manaGain = GetUInt32Value(UNIT_FIELD_STAT4) / 2;
            newMana = GetUInt32Value(UNIT_FIELD_MAXPOWER1) + manaGain;
        }

        // TODO: UNEQUIP everything and remove affects

        SetUInt32Value(PLAYER_NEXT_LEVEL_XP, nextLvlXP);
        SetUInt32Value(UNIT_FIELD_LEVEL, level);
        SetUInt32Value(UNIT_FIELD_MAXHEALTH, newHealth);
        SetUInt32Value(UNIT_FIELD_HEALTH, newHealth);
        SetUInt32Value(UNIT_FIELD_POWER1, newMana);
        SetUInt32Value(UNIT_FIELD_MAXPOWER1, newMana);

        // TODO: REEQUIP everything and add effects

        data.Initialize(SMSG_LEVELUP_INFO);

        data << uint32(level);
        data << uint32(healthGain);     // health gain
        data << uint32(manaGain);       // mana gain
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);

        // 6 new fields
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);

        WPAssert(data.size() == 48);
        GetSession()->SendPacket(&data);
    }

    // Set the update bit
    SetUInt32Value(PLAYER_XP, newXP);
}


void Player::smsg_InitialSpells()
{
    WorldPacket data;
    uint16 spellCount = m_spells.size();

    data.Initialize( SMSG_INITIAL_SPELLS );
    data << uint8(0);
    data << uint16(spellCount); // spell count

    std::list<struct spells>::iterator itr;
    for (itr = m_spells.begin(); itr != m_spells.end(); ++itr)
    {
        data << uint16(itr->spellId); // spell id
        data << uint16(itr->slotId); // slot
    }
    data << uint16(0);

    WPAssert(data.size() == 5+(4*spellCount));

    GetSession()->SendPacket(&data);

    Log::getSingleton( ).outDetail( "CHARACTER: Sent Initial Spells" );
}
bidentry* Player::GetBid(uint32 id)
{
	std::list<bidentry*>::iterator itr;
    for (itr = m_bids.begin(); itr != m_bids.end();)
    {
		if ((*itr)->AuctionID == id)
		{
			return (*itr);
		}
		else
		{
			++itr;
		}
	}
	return NULL;

}
void Player::AddBid(bidentry *be)
{
	std::list<bidentry*>::iterator itr;
    for (itr = m_bids.begin(); itr != m_bids.end();)
    {
		if ((*itr)->AuctionID == be->AuctionID)
		{
			//std::list<bidentry*>::iterator iold = itr++;
			//bidentry* b = *iold;
			m_bids.erase(itr++);
			//delete b;
		}
		else
		{
			++itr;
		}
	}
	m_bids.push_back(be);

}
void Player::RemoveMail(uint32 id)
{
	std::list<Mail*>::iterator itr;
    for (itr = m_mail.begin(); itr != m_mail.end();)
    {
		if ((*itr)->messageID == id)
		{
			m_mail.erase(itr++);
		}
		else
		{
			++itr;
		}
	}
}
void Player::AddMail(Mail *m)
{
	std::list<Mail*>::iterator itr;
    for (itr = m_mail.begin(); itr != m_mail.end();)
    {
		if ((*itr)->messageID == m->messageID)
		{
			m_mail.erase(itr++);
		}
		else
		{
			++itr;
		}
	}
	m_mail.push_back(m);
}

void Player::_SaveAuctions()
{
		std::stringstream delinvq, del;
		delinvq << "DELETE FROM auctionhouse WHERE itemowner = " << GetGUIDLow(); // TODO: use full guids				
		sDatabase.Execute( delinvq.str().c_str( ) );
		ObjectMgr::AuctionEntryMap::iterator itr;
		for (itr = objmgr.GetAuctionsBegin();itr != objmgr.GetAuctionsEnd();itr++)
		{
			AuctionEntry *Aentry = itr->second;
			if ((Aentry) && (Aentry->owner == GetGUIDLow()))
			{
				Item *it = objmgr.GetAItem(Aentry->item);
				del<< "DELETE FROM auctioned_items WHERE guid = " << it->GetGUIDLow(); // TODO: use full guids
				sDatabase.Execute( del.str().c_str( ) );			
				std::stringstream invq;
				invq <<  "INSERT INTO auctionhouse (auctioneerguid, itemguid, itemowner,buyoutprice,time,buyguid,lastbid,Id) VALUES ( " <<
                Aentry->auctioneer << ", " << Aentry->item << ", " << Aentry->owner << ", " << Aentry->buyout << ", " << Aentry->time << ", " << Aentry->bidder << ", " << Aentry->bid << ", " << Aentry->Id << " )";
				sDatabase.Execute( invq.str().c_str( ) );
				std::stringstream ss;
				ss << "INSERT INTO auctioned_items (guid, data) VALUES ("
					<< it->GetGUIDLow() << ", '"; // TODO: use full guids
				for(uint16 i = 0; i < it->GetValuesCount(); i++ )
				{
					ss << it->GetUInt32Value(i) << " ";
				}
				ss << "' )";
				sDatabase.Execute( ss.str().c_str() );
			}
		}
}
void Player::_SaveMail()
{
    std::stringstream delinvq;
    delinvq << "DELETE FROM mail WHERE reciever = " << GetGUIDLow(); // TODO: use full guids
    sDatabase.Execute( delinvq.str().c_str( ) );
	std::list<Mail*>::iterator itr;
    for (itr = m_mail.begin(); itr != m_mail.end(); itr++)
    {
			Mail *m = (*itr);
		    std::stringstream invq;
            invq <<  "INSERT INTO mail (mailId,sender,reciever,subject,body,item,time,money,COD,checked) VALUES ( " <<
                m->messageID << ", " << m->sender << ", " << m->reciever << ", '" << m->subject.c_str() << "', '" << m->body.c_str() << "', " << 
				m->item << ", " << m->time << ", " << m->money << ", " << m->COD << ", " << m->checked << " )";

			sDatabase.Execute( invq.str().c_str( ) );
	}
}
void Player::_SaveBids()
{
    std::stringstream delinvq;
    delinvq << "DELETE FROM bids WHERE bidder = " << GetGUIDLow(); // TODO: use full guids
    sDatabase.Execute( delinvq.str().c_str( ) );
	std::list<bidentry*>::iterator itr;
    for (itr = m_bids.begin(); itr != m_bids.end(); itr++)
    {
		AuctionEntry *a = objmgr.GetAuction((*itr)->AuctionID);
		if (a)
		{
		    std::stringstream invq;
            invq <<  "INSERT INTO bids (bidder, Id, amt) VALUES ( " <<
                GetGUIDLow() << ", " << (*itr)->AuctionID << ", " << (*itr)->amt << " )";

			sDatabase.Execute( invq.str().c_str( ) );
		}
	}

}
void Player::_LoadMail()
{
    // Clear spell list
    m_mail.clear();

    // Mail
    std::stringstream query;
    query << "SELECT * FROM mail WHERE reciever=" << GetGUIDLow();

    QueryResult *result = sDatabase.Query( query.str().c_str() );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();
			Mail *be = new Mail;
			be->messageID = fields[0].GetUInt32();
			be->sender = fields[1].GetUInt32();
			be->reciever = fields[2].GetUInt32();
			be->subject = fields[3].GetString();
			be->body = fields[4].GetString();
			be->item = fields[5].GetUInt32();
			be->time = fields[6].GetUInt32();
			be->money = fields[7].GetUInt32();
			be->COD = fields[8].GetUInt32();
			be->checked = fields[9].GetUInt32();
			m_mail.push_back(be);
        }
        while( result->NextRow() );

        delete result;
    }

}
void Player::_LoadBids()
{
    // Clear spell list
    m_bids.clear();

    // Spells
    std::stringstream query;
    query << "SELECT Id,amt FROM bids WHERE bidder=" << GetGUIDLow();

    QueryResult *result = sDatabase.Query( query.str().c_str() );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();
			bidentry *be = new bidentry;
			be->AuctionID = fields[0].GetUInt32();
			be->amt = fields[1].GetUInt32();
			m_bids.push_back(be);
        }
        while( result->NextRow() );

        delete result;
    }

}
void Player::addSpell(uint16 spell_id, uint16 slot_id)
{
    struct spells newspell;
    newspell.spellId = spell_id;

    if (slot_id == 0xffff)
    {
        uint16 maxid = 0;
        std::list<struct spells>::iterator itr;
        for (itr = m_spells.begin(); itr != m_spells.end(); ++itr)
        {
            if (itr->slotId > maxid) maxid = itr->slotId;
        }

        slot_id = maxid + 1;
    }

    newspell.slotId = slot_id;
    m_spells.push_back(newspell);
}
Mail* Player::GetMail(uint32 id)
{
	std::list<Mail*>::iterator itr;
    for (itr = m_mail.begin(); itr != m_mail.end(); itr++)
    {
		if ((*itr)->messageID == id)
		{
			return (*itr);
		}
	}
	return NULL;
}

//===================================================================================================================
//  Set Create Player Bits -- Sets bits required for creating a player in the updateMask.
//  Note:  Doesn't set Quest or Inventory bits
//  updateMask - the updatemask to hold the set bits
//===================================================================================================================
void Player::_SetCreateBits(UpdateMask *updateMask, Player *target) const
{
    if(target == this)
    {
        Object::_SetCreateBits(updateMask, target);
    }
    else
    {
        UpdateMask mask;
        mask.SetCount(m_valuesCount);
        _SetVisibleBits(&mask, target);

        for(uint16 index = 0; index < m_valuesCount; index++)
        {
            if(GetUInt32Value(index) != 0 && mask.GetBit(index))
                updateMask->SetBit(index);
        }
    }
}


void Player::_SetUpdateBits(UpdateMask *updateMask, Player *target) const
{
    if(target == this)
    {
        Object::_SetUpdateBits(updateMask, target);
    }
    else
    {
        UpdateMask mask;
        mask.SetCount(m_valuesCount);
        _SetVisibleBits(&mask, target);

        Object::_SetUpdateBits(updateMask, target);
        *updateMask &= mask;
    }
}


void Player::_SetVisibleBits(UpdateMask *updateMask, Player *target) const
{
    updateMask->SetBit(OBJECT_FIELD_GUID);
    updateMask->SetBit(OBJECT_FIELD_TYPE);
    updateMask->SetBit(OBJECT_FIELD_SCALE_X);

    updateMask->SetBit(UNIT_FIELD_SUMMON);
    updateMask->SetBit(UNIT_FIELD_SUMMON+1);

    updateMask->SetBit(UNIT_FIELD_TARGET);
    updateMask->SetBit(UNIT_FIELD_TARGET+1);

    updateMask->SetBit(UNIT_FIELD_HEALTH);
    updateMask->SetBit(UNIT_FIELD_POWER1);
    updateMask->SetBit(UNIT_FIELD_POWER2);
    updateMask->SetBit(UNIT_FIELD_POWER3);
    updateMask->SetBit(UNIT_FIELD_POWER4);
    updateMask->SetBit(UNIT_FIELD_POWER5);

    updateMask->SetBit(UNIT_FIELD_MAXHEALTH);
    updateMask->SetBit(UNIT_FIELD_MAXPOWER1);
    updateMask->SetBit(UNIT_FIELD_MAXPOWER2);
    updateMask->SetBit(UNIT_FIELD_MAXPOWER3);
    updateMask->SetBit(UNIT_FIELD_MAXPOWER4);
    updateMask->SetBit(UNIT_FIELD_MAXPOWER5);

    updateMask->SetBit(UNIT_FIELD_LEVEL);
    updateMask->SetBit(UNIT_FIELD_FACTIONTEMPLATE);
    updateMask->SetBit(UNIT_FIELD_BYTES_0);
    updateMask->SetBit(UNIT_FIELD_FLAGS);
    for(uint16 i = UNIT_FIELD_AURA; i < UNIT_FIELD_AURASTATE; i ++)
        updateMask->SetBit(i);
    updateMask->SetBit(UNIT_FIELD_BASEATTACKTIME);
    updateMask->SetBit(UNIT_FIELD_BASEATTACKTIME+1);
    updateMask->SetBit(UNIT_FIELD_BOUNDINGRADIUS);
    updateMask->SetBit(UNIT_FIELD_COMBATREACH);
    updateMask->SetBit(UNIT_FIELD_DISPLAYID);
    updateMask->SetBit(UNIT_FIELD_NATIVEDISPLAYID);
    updateMask->SetBit(UNIT_FIELD_MOUNTDISPLAYID);
    updateMask->SetBit(UNIT_FIELD_BYTES_1);
    updateMask->SetBit(UNIT_FIELD_MOUNTDISPLAYID);
    updateMask->SetBit(UNIT_FIELD_PETNUMBER);
    updateMask->SetBit(UNIT_FIELD_PET_NAME_TIMESTAMP);
    updateMask->SetBit(UNIT_DYNAMIC_FLAGS);

    updateMask->SetBit(PLAYER_BYTES);
    updateMask->SetBit(PLAYER_BYTES_2);
    updateMask->SetBit(PLAYER_BYTES_3);
    updateMask->SetBit(PLAYER_GUILD_TIMESTAMP);

    for(uint16 i = 0; i < EQUIPMENT_SLOT_END; i++)
    {
        updateMask->SetBit((uint16)(PLAYER_FIELD_INV_SLOT_HEAD + i*2)); // lowguid
        updateMask->SetBit((uint16)(PLAYER_FIELD_INV_SLOT_HEAD + (i*2) + 1)); // highguid
    }
}


void Player::BuildCreateUpdateBlockForPlayer( UpdateData *data, Player *target ) const
{
    for(int i = 0; i < EQUIPMENT_SLOT_END; i++)
    {
        if(m_items[i] == NULL)
            continue;

        m_items[i]->BuildCreateUpdateBlockForPlayer( data, target );
    }

    if(target == this)
    {
        for(int i = EQUIPMENT_SLOT_END; i < INVENTORY_SLOT_ITEM_END; i++)
        {
            if(m_items[i] == NULL)
                continue;

            m_items[i]->BuildCreateUpdateBlockForPlayer( data, target );
        }
    }

    Unit::BuildCreateUpdateBlockForPlayer( data, target );
}


void Player::DestroyForPlayer( Player *target ) const
{
    Unit::DestroyForPlayer( target );

    for(int i = 0; i < EQUIPMENT_SLOT_END; i++)
    {
        if(m_items[i] == NULL)
            continue;

        m_items[i]->DestroyForPlayer( target );
    }

    if(target == this)
    {
        for(int i = EQUIPMENT_SLOT_END; i < INVENTORY_SLOT_ITEM_END; i++)
        {
            if(m_items[i] == NULL)
                continue;

            m_items[i]->DestroyForPlayer( target );
        }
    }
}


void Player::SaveToDB()
{
    if (m_dismountTimer != 0)
    {
        SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID , 0);
        RemoveFlag( UNIT_FIELD_FLAGS ,0x000004 );
        RemoveFlag( UNIT_FIELD_FLAGS, 0x002000 );
    }

    bool inworld = IsInWorld();
    if (inworld)
        RemoveFromWorld();

    _RemoveAllItemMods();
    _RemoveAllAffectMods();

    std::stringstream ss;
    ss << "DELETE FROM characters WHERE guid = " << GetGUIDLow();
    sDatabase.Execute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "INSERT INTO characters (guid, acct, name, mapId, zoneId, positionX, positionY, positionZ, orientation, data, taximask) VALUES ("
        << GetGUIDLow() << ", " // TODO: use full guids
        << GetSession()->GetAccountId() << ", '"
        << m_name << "', "
        << m_mapId << ", "
        << m_zoneId << ", "
        << m_positionX << ", "
        << m_positionY << ", "
        << m_positionZ << ", "
        << m_orientation << ", '";

    uint16 i;
    for( i = 0; i < m_valuesCount; i++ )
        ss << GetUInt32Value(i) << " ";

    ss << "', '";

    for( i = 0; i < 8; i++ )
        ss << m_taximask[i] << " ";

    ss << "' )";

    sDatabase.Execute( ss.str().c_str() );

    // TODO: equip all items and apply affects


	//Mail
	_SaveMail();

	//bids
	_SaveBids();

	//Auctions
	_SaveAuctions();

    // inventory
    _SaveInventory();

    // save quest progress
    _SaveQuestStatus();

    // spells
    _SaveSpells();

    // affects
    _SaveAffects();

    _ApplyAllAffectMods();
    _ApplyAllItemMods();

    if (inworld)
        AddToWorld();
}


void Player::_SaveQuestStatus()
{
    std::stringstream ss;
    ss << "DELETE FROM queststatus WHERE playerId = " << GetGUIDLow();
    sDatabase.Execute( ss.str().c_str() );

    for( StatusMap::iterator i = mQuestStatus.begin( ); i != mQuestStatus.end( ); ++ i )
    {
        std::stringstream ss2;
        ss2 << "INSERT INTO queststatus (playerId,questId,status,questMobCount1,questMobCount2,questMobCount3,questMobCount4,"
            << "questItemCount1,questItemCount2,questItemCount3,questItemCount4) VALUES "
            << "(" << GetGUIDLow() << ", "
            << i->first << ", "
            << i->second.status << ", "
            << i->second.m_questMobCount[0] << ", "
            << i->second.m_questMobCount[1] << ", "
            << i->second.m_questMobCount[2] << ", "
            << i->second.m_questMobCount[3] << ", "
            << i->second.m_questItemCount[0] << ", "
            << i->second.m_questItemCount[1] << ", "
            << i->second.m_questItemCount[2] << ", "
            << i->second.m_questItemCount[3]
            << ")";

            sDatabase.Execute( ss2.str().c_str() );
    }
}


void Player::_SaveInventory()
{
    std::stringstream delinvq;
    delinvq << "DELETE FROM inventory WHERE player_guid = " << GetGUIDLow(); // TODO: use full guids
    sDatabase.Execute( delinvq.str().c_str( ) );

    for(unsigned int i = 0; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        if (m_items[i] != 0)
        {
            m_items[i]->SaveToDB();

            std::stringstream invq;
            invq <<  "INSERT INTO inventory (player_guid, slot, item_guid) VALUES ( " <<
                GetGUIDLow() << ", " << i << ", " << m_items[i]->GetGUIDLow() << " )";

            sDatabase.Execute( invq.str().c_str( ) );
        }
    }
}


void Player::_SaveSpells()
{
    std::stringstream query;
    query << "DELETE FROM char_spells WHERE charId = " << GetGUIDLow(); // TODO: use full guids
    sDatabase.Execute( query.str().c_str() );

    std::list<struct spells>::iterator itr;
    for (itr = m_spells.begin(); itr != m_spells.end(); ++itr)
    {
        query.rdbuf()->str("");
        query << "INSERT INTO char_spells (charId,spellId,slotId) VALUES ( "
            << GetGUIDLow() << ", " << itr->spellId << ", " << itr->slotId << " )";

        sDatabase.Execute( query.str().c_str() );
    }
}


void Player::_SaveAffects()
{
}


// NOTE: 32bit guids are only for compatibility with older bases.
void Player::LoadFromDB( uint32 guid )
{
    std::stringstream ss;
    ss << "SELECT * FROM characters WHERE guid=" << guid;

    QueryResult *result = sDatabase.Query( ss.str().c_str() );
    ASSERT(result);

    Field *fields = result->Fetch();

    Object::_Create( guid, HIGHGUID_PLAYER );

    LoadValues( fields[2].GetString() );

    // TODO: check for overflow
    m_name = fields[3].GetString();

    m_positionX = fields[4].GetFloat();
    m_positionY = fields[5].GetFloat();
    m_positionZ = fields[6].GetFloat();
    m_mapId = fields[7].GetUInt32();
    m_zoneId = fields[8].GetUInt32();
    m_orientation = fields[9].GetFloat();

    if( HasFlag(PLAYER_FLAGS, 0x10) )
        m_deathState = DEAD;

    LoadTaxiMask( fields[10].GetString() );

    delete result;

    /*
    m_outfitId = atoi( row[ 11 ] );
    m_guildId = atoi( row[ 17 ] );
    m_petInfoId = atoi( row[ 18 ] );
    m_petLevel = atoi( row[ 19 ] );
    m_petFamilyId = atoi( row[ 20 ] );
    */

	_LoadMail();

    _LoadInventory();

    _LoadSpells();

    _LoadQuestStatus();

	_LoadBids();

    _LoadAffects();

    _ApplyAllAffectMods();
    _ApplyAllItemMods();
}


void Player::_LoadInventory()
{
    // Clean current inventory
    for(uint16 i = 0; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        if(m_items[i])
        {
            delete m_items[i];

            SetUInt64Value((uint16)(PLAYER_FIELD_INV_SLOT_HEAD + i*2), 0);
            m_items[i] = 0;
        }
    }

    // Inventory
    std::stringstream invq;
    invq << "SELECT item_guid, slot FROM inventory WHERE player_guid=" << GetGUIDLow();

    QueryResult *result = sDatabase.Query( invq.str().c_str() );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();

            Item* item = new Item;
            item->LoadFromDB(fields[0].GetUInt32(),1);

            AddItemToSlot( (uint8)fields[1].GetUInt16(), item);
        }
        while( result->NextRow() );

        delete result;
    }
}
bool Player::HasSpell(uint32 spell)
{
    std::list<struct spells>::iterator itr;
    for (itr = m_spells.begin(); itr != m_spells.end(); ++itr)
    {
		if (itr->spellId == spell)
		{
			return true;
		}
	}
	return false;

}
void Player::_LoadSpells()
{
    // Clear spell list
    m_spells.clear();

    // Spells
    std::stringstream query;
    query << "SELECT spellId, slotId FROM char_spells WHERE charId=" << GetGUIDLow();

    QueryResult *result = sDatabase.Query( query.str().c_str() );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();

            addSpell(fields[0].GetUInt16(), fields[1].GetUInt16());
        }
        while( result->NextRow() );

        delete result;
    }
}


void Player::_LoadQuestStatus()
{
    // clear list
    mQuestStatus.clear();

    std::stringstream ss;
    ss << "SELECT * FROM queststatus WHERE playerId=" << GetGUIDLow();

    QueryResult *result = sDatabase.Query( ss.str().c_str() );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();

            quest_status qs;
            qs.quest_id = fields[1].GetUInt32();
            qs.status = fields[2].GetUInt32();
            qs.m_questMobCount[0] = fields[3].GetUInt32();
            qs.m_questMobCount[1] = fields[4].GetUInt32();
            qs.m_questMobCount[2] = fields[5].GetUInt32();
            qs.m_questMobCount[3] = fields[6].GetUInt32();
            qs.m_questItemCount[0] = fields[7].GetUInt32();
            qs.m_questItemCount[1] = fields[8].GetUInt32();
            qs.m_questItemCount[2] = fields[9].GetUInt32();
            qs.m_questItemCount[3] = fields[10].GetUInt32();

            loadExistingQuest(qs);
        }
        while( result->NextRow() );

        delete result;
    }
}


void Player::_LoadAffects()
{
}


void Player::DeleteFromDB()
{
    std::stringstream ss;

    ss << "DELETE FROM characters WHERE guid = " << GetGUIDLow();
    sDatabase.Execute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "DELETE FROM char_spells WHERE charid = " << GetGUIDLow();
    sDatabase.Execute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "DELETE FROM inventory WHERE player_guid = " << GetGUIDLow();
    sDatabase.Execute( ss.str( ).c_str( ) );

    for(int i = 0; i < EQUIPMENT_SLOT_END; i++)
    {
        if(m_items[i] == NULL)
            continue;

        m_items[i]->DeleteFromDB();
    }

    ss.rdbuf()->str("");
    ss << "DELETE FROM queststatus WHERE playerId = " << GetGUIDLow();
    sDatabase.Execute( ss.str( ).c_str( ) );
}


uint8 Player::FindFreeItemSlot(uint32 type)
{
    switch(type)
    {
    case INVTYPE_NON_EQUIP:
        return INVENTORY_SLOT_ITEM_END; // ???
    case INVTYPE_HEAD:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_HEAD))
                return EQUIPMENT_SLOT_HEAD;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_NECK:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_NECK))
                return EQUIPMENT_SLOT_NECK;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_SHOULDERS:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_SHOULDERS))
                return EQUIPMENT_SLOT_SHOULDERS;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_BODY:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_BODY))
                return EQUIPMENT_SLOT_BODY;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_CHEST:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_CHEST))
                return EQUIPMENT_SLOT_CHEST;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_ROBE: // ???
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_CHEST))
                return EQUIPMENT_SLOT_CHEST;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_WAIST:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_WAIST))
                return EQUIPMENT_SLOT_WAIST;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_LEGS:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_LEGS))
                return EQUIPMENT_SLOT_LEGS;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_FEET:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_FEET))
                return EQUIPMENT_SLOT_FEET;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_WRISTS:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_WRISTS))
                return EQUIPMENT_SLOT_WRISTS;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_HANDS:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_HANDS))
                return EQUIPMENT_SLOT_HANDS;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_FINGER:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_FINGER1))
                return EQUIPMENT_SLOT_FINGER1;
            else if (!GetItemBySlot(EQUIPMENT_SLOT_FINGER2))
                return EQUIPMENT_SLOT_FINGER2;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_TRINKET:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_TRINKET1))
                return EQUIPMENT_SLOT_TRINKET1;
            else if (!GetItemBySlot(EQUIPMENT_SLOT_TRINKET2))
                return EQUIPMENT_SLOT_TRINKET2;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_CLOAK:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_BACK))
                return EQUIPMENT_SLOT_BACK;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_WEAPON:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_MAINHAND) )
                return EQUIPMENT_SLOT_MAINHAND;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_SHIELD:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_OFFHAND))
                return EQUIPMENT_SLOT_OFFHAND;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_RANGED:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_RANGED))
                return EQUIPMENT_SLOT_RANGED;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_2HWEAPON:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_MAINHAND) && !GetItemBySlot(EQUIPMENT_SLOT_OFFHAND))
                return EQUIPMENT_SLOT_MAINHAND;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_TABARD:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_TABARD))
                return EQUIPMENT_SLOT_TABARD;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_WEAPONMAINHAND:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_MAINHAND))
                return EQUIPMENT_SLOT_MAINHAND;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_WEAPONOFFHAND:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_OFFHAND))
                return EQUIPMENT_SLOT_OFFHAND;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_HOLDABLE:
        {
            if (!GetItemBySlot(EQUIPMENT_SLOT_MAINHAND))
                return EQUIPMENT_SLOT_MAINHAND;
            else
                return INVENTORY_SLOT_ITEM_END;
        }
    case INVTYPE_AMMO:
        return EQUIPMENT_SLOT_RANGED; // ?
    case INVTYPE_THROWN:
        return EQUIPMENT_SLOT_RANGED; // ?
    case INVTYPE_RANGEDRIGHT:
        return EQUIPMENT_SLOT_RANGED; // ?
    case INVTYPE_BAG:
        {
            for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
            {
                if (!GetItemBySlot(i))
                    return i;
            }
            return INVENTORY_SLOT_ITEM_END;
        }
    default:
        ASSERT(0);
        return INVENTORY_SLOT_ITEM_END;
    }
}


bool Player::CanEquipItemInSlot(uint8 slot, uint32 type)
{
    switch(slot)
    {
    case EQUIPMENT_SLOT_HEAD:
        {
            if(type == INVTYPE_HEAD)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_NECK:
        {
            if(type == INVTYPE_NECK)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_SHOULDERS:
        {
            if(type == INVTYPE_SHOULDERS)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_BODY:
        {
            if(type == INVTYPE_BODY)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_CHEST:
        {
            if(type == INVTYPE_CHEST || type == INVTYPE_ROBE)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_WAIST:
        {
            if(type == INVTYPE_WAIST)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_LEGS:
        {
            if(type == INVTYPE_LEGS)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_FEET:
        {
            if(type == INVTYPE_FEET)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_WRISTS:
        {
            if(type == INVTYPE_WRISTS)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_HANDS:
        {
            if(type == INVTYPE_HANDS)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_FINGER1:
    case EQUIPMENT_SLOT_FINGER2:
        {
            if(type == INVTYPE_FINGER)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_TRINKET1:
    case EQUIPMENT_SLOT_TRINKET2:
        {
            if(type == INVTYPE_TRINKET)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_BACK:
        {
            if(type == INVTYPE_CLOAK)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_MAINHAND:
        {
            if(type == INVTYPE_WEAPON || type == INVTYPE_WEAPONMAINHAND || type == INVTYPE_HOLDABLE ||
                (type == INVTYPE_2HWEAPON && !GetItemBySlot(EQUIPMENT_SLOT_OFFHAND)))
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_OFFHAND:
        {
            if((type == INVTYPE_WEAPON || type == INVTYPE_SHIELD || type == INVTYPE_WEAPONOFFHAND) &&
                ((!GetItemBySlot(EQUIPMENT_SLOT_MAINHAND) ||
                GetItemBySlot(EQUIPMENT_SLOT_MAINHAND)->GetProto()->InventoryType != INVTYPE_2HWEAPON)))
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_RANGED:
        {
            if(type == INVTYPE_AMMO || type == INVTYPE_THROWN || type == INVTYPE_RANGEDRIGHT)
                return true;
            else
                return false;
        }
    case EQUIPMENT_SLOT_TABARD:
        {
            if(type == INVTYPE_TABARD)
                return true;
            else
                return false;
        }
    case INVENTORY_SLOT_BAG_1:
    case INVENTORY_SLOT_BAG_2:
    case INVENTORY_SLOT_BAG_3:
    case INVENTORY_SLOT_BAG_4:
        {
            if(type == INVTYPE_BAG)
                return true;
            else
                return false;
        }
    case INVENTORY_SLOT_ITEM_1:
    case INVENTORY_SLOT_ITEM_2:
    case INVENTORY_SLOT_ITEM_3:
    case INVENTORY_SLOT_ITEM_4:
    case INVENTORY_SLOT_ITEM_5:
    case INVENTORY_SLOT_ITEM_6:
    case INVENTORY_SLOT_ITEM_7:
    case INVENTORY_SLOT_ITEM_8:
    case INVENTORY_SLOT_ITEM_9:
    case INVENTORY_SLOT_ITEM_10:
    case INVENTORY_SLOT_ITEM_11:
    case INVENTORY_SLOT_ITEM_12:
    case INVENTORY_SLOT_ITEM_13:
    case INVENTORY_SLOT_ITEM_14:
    case INVENTORY_SLOT_ITEM_15:
    case INVENTORY_SLOT_ITEM_16:
        {
            return true;
        }
    default:
        return false;
    }
}


void Player::SwapItemSlots(uint8 srcslot, uint8 dstslot)
{
    ASSERT(srcslot < INVENTORY_SLOT_ITEM_END);
    ASSERT(dstslot < INVENTORY_SLOT_ITEM_END);

    Item *temp;
    temp = m_items[srcslot];
    m_items[srcslot] = m_items[dstslot];
    m_items[dstslot] = temp;

    if ( IsInWorld() )
    {
        if ( srcslot < EQUIPMENT_SLOT_END && dstslot >= EQUIPMENT_SLOT_END )
        {
            for(Object::InRangeSet::iterator i = GetInRangeSetBegin();
                i != GetInRangeSetEnd(); i++)
            {
                if((*i)->GetTypeId() == TYPEID_PLAYER)
                    m_items[dstslot]->DestroyForPlayer( (Player*)*i );
            }
        }
        else if ( srcslot >= EQUIPMENT_SLOT_END && dstslot < EQUIPMENT_SLOT_END )
        {
            UpdateData upd;
            WorldPacket packet;

            for(Object::InRangeSet::iterator i = GetInRangeSetBegin();
                i != GetInRangeSetEnd(); i++)
            {
                if((*i)->GetTypeId() == TYPEID_PLAYER)
                {
                    upd.Clear();
                    m_items[dstslot]->BuildCreateUpdateBlockForPlayer( &upd, (Player*)*i );
                    upd.BuildPacket( &packet );
                    GetSession()->SendPacket( &packet );
                }
            }
        }
    }

    SetUInt64Value( (uint16)(PLAYER_FIELD_INV_SLOT_HEAD  + (dstslot*2)),
        m_items[dstslot] ? m_items[dstslot]->GetGUID() : 0 );
    SetUInt64Value( (uint16)(PLAYER_FIELD_INV_SLOT_HEAD  + (srcslot*2)),
        m_items[srcslot] ? m_items[srcslot]->GetGUID() : 0 );

    if (srcslot >= EQUIPMENT_SLOT_END && dstslot < EQUIPMENT_SLOT_END)
        _ApplyItemMods(m_items[dstslot], true);
}

uint32 Player::GetSlotByItemID(uint32 ID)
{
    for(uint32 i=INVENTORY_SLOT_ITEM_START;i<INVENTORY_SLOT_ITEM_END;i++){
        if(m_items[i] != 0)
            if(m_items[i]->GetProto()->ItemId == ID)
                return i;
    }
    return 0;
}
uint32 Player::GetSlotByItemGUID(uint64 guid)
{
    for(uint32 i=0;i<INVENTORY_SLOT_ITEM_END;i++){
        if(m_items[i] != 0)
            if(m_items[i]->GetGUID() == guid)
                return i;
    }
    return 0;


}
void Player::AddItemToSlot(uint8 slot, Item *item)
{
    ASSERT(slot < INVENTORY_SLOT_ITEM_END);
    ASSERT(m_items[slot] == NULL);

    if( IsInWorld() )
    {
        UpdateData upd;
        WorldPacket packet;

        // create for ourselves
        item->BuildCreateUpdateBlockForPlayer( &upd, this );
        upd.BuildPacket( &packet );
        GetSession()->SendPacket( &packet );

        if ( slot < EQUIPMENT_SLOT_END )
        {
            for(Object::InRangeSet::iterator i = GetInRangeSetBegin();
                i != GetInRangeSetEnd(); i++)
            {
                if((*i)->GetTypeId() == TYPEID_PLAYER)
                {
                    upd.Clear();
                    item->BuildCreateUpdateBlockForPlayer( &upd, (Player*)*i );
                    upd.BuildPacket( &packet );
                    GetSession()->SendPacket( &packet );
                }
            }
        }

       
    }

    m_items[slot] = item;
    SetUInt64Value( (uint16)(PLAYER_FIELD_INV_SLOT_HEAD  + (slot*2)), m_items[slot] ? m_items[slot]->GetGUID() : 0 );

    item->SetOwner( this );
    item->PlaceOnMap();

    if ( slot < EQUIPMENT_SLOT_END )
	{
        int VisibleBase = PLAYER_VISIBLE_ITEM_1_0 + (slot * 9);
       	SetUInt32Value(VisibleBase, item->GetUInt32Value(OBJECT_FIELD_ENTRY));
       	SetUInt32Value(VisibleBase + 1, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT));
       	SetUInt32Value(VisibleBase + 2, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 3));
       	SetUInt32Value(VisibleBase + 3, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 6));
       	SetUInt32Value(VisibleBase + 4, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 9));
       	SetUInt32Value(VisibleBase + 5, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 12));
       	SetUInt32Value(VisibleBase + 6, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 15));
       	SetUInt32Value(VisibleBase + 7, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 18));
       	SetUInt32Value(VisibleBase + 8, item->GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID));
        _ApplyItemMods( item, true );
	}
}


Item* Player::RemoveItemFromSlot(uint8 slot)
{
    ASSERT(slot < INVENTORY_SLOT_ITEM_END);

    Item *item = m_items[slot];
    m_items[slot] = NULL;
    SetUInt64Value( (uint16)(PLAYER_FIELD_INV_SLOT_HEAD  + (slot*2)), 0 );

    if ( slot < EQUIPMENT_SLOT_END )
        _ApplyItemMods( item, false );

    item->SetOwner( NULL );

    if ( IsInWorld() )
    {
        item->RemoveFromMap();

        // create for ourselves
        item->DestroyForPlayer( this );

        if ( slot < EQUIPMENT_SLOT_END )
        {
            for(Object::InRangeSet::iterator i = GetInRangeSetBegin();
                i != GetInRangeSetEnd(); i++)
            {
                if((*i)->GetTypeId() == TYPEID_PLAYER)
                    item->DestroyForPlayer( (Player*)*i );
            }
        }
    }

    return item;
}


void Player::AddToWorld()
{
    Object::AddToWorld();

    for(int i = 0; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        if(m_items[i])
            m_items[i]->AddToWorld();
    }
}


void Player::RemoveFromWorld()
{
    for(int i = 0; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        if(m_items[i])
            m_items[i]->RemoveFromWorld();
    }

    Object::RemoveFromWorld();
}

// TODO: perhaps item should just have a list of mods, that will simplify code
void Player::_ApplyItemMods(Item *item, bool apply)
{
     ASSERT(item);
    ItemPrototype *proto = item->GetProto();
	if (apply)
	{
		Log::getSingleton().outString("applying mods for item %u ",item->GetGUIDLow());
	}
	else
	{
		Log::getSingleton().outString("removing mods for item %u ",item->GetGUIDLow());
	}
    // FIXME: just an example
    if (proto->ArcaneRes)
        SetUInt32Value(UNIT_FIELD_RESISTANCES+5, GetUInt32Value(UNIT_FIELD_RESISTANCES+5) +
        (apply ? proto->ArcaneRes : -(int32)proto->ArcaneRes));
    if (proto->FireRes)
        SetUInt32Value(UNIT_FIELD_RESISTANCES+1, GetUInt32Value(UNIT_FIELD_RESISTANCES+1) +
        (apply ? proto->FireRes : -(int32)proto->FireRes));
    if (proto->NatureRes)
        SetUInt32Value(UNIT_FIELD_RESISTANCES+2, GetUInt32Value(UNIT_FIELD_RESISTANCES+2) +
        (apply ? proto->NatureRes : -(int32)proto->NatureRes));
    if (proto->FrostRes)
        SetUInt32Value(UNIT_FIELD_RESISTANCES+3, GetUInt32Value(UNIT_FIELD_RESISTANCES+3) +
        (apply ? proto->FrostRes : -(int32)proto->FrostRes));
    if (proto->ShadowRes)
        SetUInt32Value(UNIT_FIELD_RESISTANCES+4, GetUInt32Value(UNIT_FIELD_RESISTANCES+4) +
        (apply ? proto->ShadowRes : -(int32)proto->ShadowRes));
	if (proto->Armor)
	{
        SetUInt32Value(UNIT_FIELD_RESISTANCES+0, GetUInt32Value(UNIT_FIELD_RESISTANCES+0) +
        (apply ? proto->Armor : -(int32)proto->Armor));
	}
	if (proto->DamageMin[0])
	{
        SetFloatValue(UNIT_FIELD_MINDAMAGE, GetFloatValue(UNIT_FIELD_MINDAMAGE) +
        (apply ? proto->DamageMin[0] : -proto->DamageMin[0]));
		if (apply)
		{
			Log::getSingleton().outString("adding %f mindam ",proto->DamageMin[0]);
		}
		else
		{
			Log::getSingleton().outString("removing %f mindamn ",proto->DamageMin[0]);
		}
	}
	if (proto->DamageMax[0])
	{
        SetFloatValue(UNIT_FIELD_MAXDAMAGE, GetFloatValue(UNIT_FIELD_MAXDAMAGE) +
        (apply ? proto->DamageMax[0] : -proto->DamageMax[0]));
		if (apply)
		{
			Log::getSingleton().outString("adding %f maxdam ",proto->DamageMax[0]);
		}
		else
		{
			Log::getSingleton().outString("removing %f maxdam ",proto->DamageMax[0]);
		}
	}
	if (proto->Delay)
	{
		SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, apply ? proto->Delay : 2);
		SetUInt32Value(UNIT_FIELD_BASEATTACKTIME + 1, apply ? proto->Delay : 2);
	}
}


void Player::_RemoveAllItemMods()
{
    for (int i = 0; i < EQUIPMENT_SLOT_END; i++)
    {
        if(m_items[i])
            _ApplyItemMods(m_items[i], false);
    }
}


void Player::_ApplyAllItemMods()
{
    /*for (int i = 0; i < EQUIPMENT_SLOT_END; i++)
    {
        if(m_items[i])
            _ApplyItemMods(m_items[i], true);
    }*/
}

void Player::SetMovement(uint8 pType)
{
    WorldPacket data;

    switch(pType)
    {
    case MOVE_ROOT:
        {
            data.Initialize(SMSG_FORCE_MOVE_ROOT);
            data << GetGUID();
            GetSession()->SendPacket( &data );
        }break;
    case MOVE_UNROOT:
        {
            data.Initialize(SMSG_FORCE_MOVE_UNROOT);
            data << GetGUID();
            GetSession()->SendPacket( &data );
        }break;
    case MOVE_WATER_WALK:
        {
            data.Initialize(SMSG_MOVE_WATER_WALK);
            data << GetGUID();
            GetSession()->SendPacket( &data );
        }break;
    case MOVE_LAND_WALK:
        {
            data.Initialize(SMSG_MOVE_LAND_WALK);
            data << GetUInt32Value( OBJECT_FIELD_GUID );
            GetSession()->SendPacket( &data );
        }break;
    default:break;
    }
}

void Player::SetPlayerSpeed(uint8 SpeedType, float value, bool forced)
{
     WorldPacket data;

     switch(SpeedType)
     {
     case RUN:
         {
             if(forced) { data.Initialize(SMSG_FORCE_RUN_SPEED_CHANGE); }
             else { data.Initialize(MSG_MOVE_SET_RUN_SPEED); }
             data << GetGUID();
             data << float(value);
             GetSession()->SendPacket( &data );
         }break;
     case RUNBACK:
         {
             if(forced) { data.Initialize(SMSG_FORCE_RUN_BACK_SPEED_CHANGE); }
             else { data.Initialize(MSG_MOVE_SET_RUN_BACK_SPEED); }
             data << GetGUID();
             data << float(value);
             GetSession()->SendPacket( &data );
         }break;
     case SWIM:
         {
             if(forced) { data.Initialize(SMSG_FORCE_SWIM_SPEED_CHANGE); }
             else { data.Initialize(MSG_MOVE_SET_SWIM_SPEED); }
             data << GetGUID();
             data << float(value);
             GetSession()->SendPacket( &data );
         }break;
     case SWIMBACK:
         {
             data.Initialize(MSG_MOVE_SET_SWIM_BACK_SPEED);
             data << GetGUID();
             data << float(value);
             GetSession()->SendPacket( &data );
         }break;
     default:break;
     }
}

void Player::BuildPlayerRepop()
{
    WorldPacket data;
    //1.1.1
    SetUInt32Value( UNIT_FIELD_HEALTH, 1 );

    SetMovement(MOVE_UNROOT);
    SetMovement(MOVE_WATER_WALK);

    SetPlayerSpeed(RUN, (float)8.5, true);
    SetPlayerSpeed(SWIM, (float)5.9, true);

    data.Initialize(SMSG_CORPSE_RECLAIM_DELAY );
    data << uint8(0x30) << uint8(0x75) << uint8(0x00) << uint8(0x00);
    GetSession()->SendPacket( &data );

    data.Initialize(SMSG_SPELL_START );
    data << GetGUID() << GetGUID() << uint32(8326);
    data << uint16(0) << uint32(0) << uint16(0x02) << uint32(0) << uint32(0);
    GetSession()->SendPacket( &data );

    data.Initialize(SMSG_UPDATE_AURA_DURATION);
    data << uint8(32);
    data << uint32(0);
    GetSession()->SendPacket( &data );

    data.Initialize(SMSG_CAST_RESULT);
    data << uint32(8326);
    data << uint8(0x00);
    GetSession()->SendPacket( &data );

    data.Initialize(SMSG_SPELL_GO);
    data << GetGUID() << GetGUID() << uint32(8326);
    data << uint16(01) << uint8(0) << uint8(0);
    data << uint16(0040);
    data << GetPositionX();
    data << GetPositionY();
    data << GetPositionZ();
    GetSession()->SendPacket( &data );

    data.Initialize(SMSG_SPELLLOGEXECUTE);
    data << (uint32)GetGUID() << (uint32)GetGUID();
    data << uint32(8326);
    data << uint32(1);
    data << uint32(0x24);
    data << uint32(1);
    data << GetGUID();
    GetSession()->SendPacket( &data );

    data.Initialize(SMSG_STOP_MIRROR_TIMER);
    data << uint8(0x00) << uint8(0x00) << uint8(0x00) << uint8(0x00);
    GetSession()->SendPacket( &data );

    data.Initialize(SMSG_STOP_MIRROR_TIMER);
    data << uint8(0x01) << uint8(0x00) << uint8(0x00) << uint8(0x00);
    GetSession()->SendPacket( &data );

    data.Initialize(SMSG_STOP_MIRROR_TIMER);
    data << uint8(0x02) << uint8(0x00) << uint8(0x00) << uint8(0x00);
    GetSession()->SendPacket( &data );

    SetUInt32Value(CONTAINER_FIELD_SLOT_1+29, 8326);
    SetUInt32Value(UNIT_FIELD_AURA+32, 8326);
    SetUInt32Value(UNIT_FIELD_AURALEVELS+8, 0xeeeeee00);
    SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS+8, 0xeeeeee00);
    SetUInt32Value(UNIT_FIELD_AURAFLAGS+4, 12);
    SetUInt32Value(UNIT_FIELD_AURASTATE, 2);

    SetFlag(PLAYER_FLAGS, 0x10);

    //spawn Corpse
    SpawnCorpseBody();
}

void Player::ResurrectPlayer()
{
    RemoveFlag(PLAYER_FLAGS, 0x10);
    setDeathState(ALIVE);
}

void Player::KillPlayer()
{
    WorldPacket data;

    SetMovement(MOVE_ROOT);

    data.Initialize(SMSG_STOP_MIRROR_TIMER);
    data << uint8(0x00) << uint8(0x00) << uint8(0x00) << uint8(0x00);
    GetSession()->SendPacket( &data );

    data.Initialize(SMSG_STOP_MIRROR_TIMER);
    data << uint8(0x01) << uint8(0x00) << uint8(0x00) << uint8(0x00);
    GetSession()->SendPacket( &data );

    data.Initialize(SMSG_STOP_MIRROR_TIMER);
    data << uint8(0x02) << uint8(0x00) << uint8(0x00) << uint8(0x00);
    GetSession()->SendPacket( &data );

    setDeathState(CORPSE);
    SetFlag( UNIT_FIELD_FLAGS, 0x08 ); //player death animation, also can be used with DYNAMIC_FLAGS
    SetFlag( UNIT_DYNAMIC_FLAGS, 0x00 );
    CreateCorpse();
}

void Player::CreateCorpse()
{
    Corpse *pCorpse;
    uint32 _uf, _pb, _pb2, _cfb1, _cfb2;

    pCorpse = objmgr.GetCorpseByOwner(this);
    if(!pCorpse)
    {
        pCorpse = new Corpse();
        pCorpse->Create(objmgr.GenerateLowGuid(HIGHGUID_CORPSE), this, GetMapId(), GetPositionX(),
            GetPositionY(), GetPositionZ(), GetOrientation());

        _uf = GetUInt32Value(UNIT_FIELD_BYTES_0);
        _pb = GetUInt32Value(PLAYER_BYTES);
        _pb2 = GetUInt32Value(PLAYER_BYTES_2);

        uint8 race       = (uint8)(_uf);
        uint8 skin       = (uint8)(_pb);
        uint8 face       = (uint8)(_pb >> 8);
        uint8 hairstyle  = (uint8)(_pb >> 16);
        uint8 haircolor  = (uint8)(_pb >> 24);
        uint8 facialhair = (uint8)(_pb2);

        _cfb1 = ((0x00) | (race << 8) | (0x00 << 16) | (skin << 24));
        _cfb2 = ((face) | (hairstyle << 8) | (haircolor << 16) | (facialhair << 24));

        pCorpse->SetZoneId( GetZoneId() );
        pCorpse->SetUInt32Value( CORPSE_FIELD_BYTES_1, _cfb1 );
        pCorpse->SetUInt32Value( CORPSE_FIELD_BYTES_2, _cfb2 );
        pCorpse->SetUInt32Value( CORPSE_FIELD_FLAGS, 4 );
        pCorpse->SetUInt32Value( CORPSE_FIELD_DISPLAY_ID, GetUInt32Value(UNIT_FIELD_DISPLAYID) );

        uint32 iDisplayID;
        uint16 iIventoryType;
        uint32 _cfi;
        for (int i = 0; i < EQUIPMENT_SLOT_END; i++)
        {
            if(m_items[i])
            {
                iDisplayID = m_items[i]->GetProto()->DisplayInfoID;
                iIventoryType = (uint16)m_items[i]->GetProto()->InventoryType;

                _cfi =  (uint16(iDisplayID)) | (iIventoryType)<< 24;
                pCorpse->SetUInt32Value(CORPSE_FIELD_ITEM + i,_cfi);
            }
        }
        //save corpse in db for future use
        pCorpse->SaveToDB();
        objmgr.AddObject(pCorpse);
    }
    else //Corpse already exist in world, update it
    {
        pCorpse->SetPosition(GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation());
    }
}

void Player::SpawnCorpseBody()
{
    Corpse *pCorpse;

    pCorpse = objmgr.GetCorpseByOwner(this);
    if(pCorpse && !pCorpse->IsInWorld())
        pCorpse->PlaceOnMap();
}

void Player::SpawnCorpseBones()
{
    Corpse *pCorpse;
    pCorpse = objmgr.GetCorpseByOwner(this);
    if(pCorpse)
    {
        pCorpse->SetUInt32Value(CORPSE_FIELD_FLAGS, 5);
        pCorpse->SetUInt64Value(CORPSE_FIELD_OWNER, 0); // remove corpse owner association
        //remove item association
        for (int i = 0; i < EQUIPMENT_SLOT_END; i++)
        {
            if(pCorpse->GetUInt32Value(CORPSE_FIELD_ITEM + i))
                pCorpse->SetUInt32Value(CORPSE_FIELD_ITEM + i, 0);
        }
        pCorpse->DeleteFromDB();
    }
}

void Player::DeathDurabilityLoss(double percent)
{
    uint32 pDurability, pNewDurability;

    for (int i = 0; i < EQUIPMENT_SLOT_END; i++)
    {
        if(m_items[i])
        {
            pDurability =  m_items[i]->GetUInt32Value(ITEM_FIELD_DURABILITY);
            if(pDurability)
            {
                pNewDurability = (uint32)(pDurability*percent);
                pNewDurability = (pDurability - pNewDurability);
                if(pNewDurability < 0) { pNewDurability = 0; }

                m_items[i]->SetUInt32Value(ITEM_FIELD_DURABILITY, pNewDurability);
            }
        }
    }
}


void Player::RepopAtGraveyard()
{
    float closestX, closestY, closestZ, closestO;
    float curX, curY, curZ;
    bool first = true;

    ObjectMgr::GraveyardMap::const_iterator itr;
    for (itr = objmgr.GetGraveyardListBegin(); itr != objmgr.GetGraveyardListEnd(); itr++)
    {
        GraveyardTeleport *pGrave = itr->second;
        if(pGrave->MapId == GetMapId())
        {
            curX = pGrave->X;
            curY = pGrave->Y;
            curZ = pGrave->Z;
            if( first || pow(m_positionX-curX,2) + pow(m_positionY-curY,2) <
                pow(m_positionX-closestX,2) + pow(m_positionY-closestY,2) )
            {
                first = false;

                closestX = curX;
                closestY = curY;
                closestZ = curZ;
                closestO = pGrave->O;
            }
        }
    }

    if(closestX && closestY && closestZ)
        SetPosition(closestX,closestY,closestZ,closestO, false);
}

void Player::JoinedChannel(Channel *c)
{
	m_channels.push_back(c);
}
void Player::LeftChannel(Channel *c)
{
	m_channels.remove(c);
}
void Player::CleanupChannels()
{
	list<Channel *>::iterator i;
	for(i = m_channels.begin(); i != m_channels.end(); i++)
		(*i)->Leave(this,false);
}
