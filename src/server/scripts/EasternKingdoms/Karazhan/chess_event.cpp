/**********************************************************************************************
*                                                                                             *
*                          ______ _  __   _____ ____  _____  ______                           *
*                         |  ____| |/ /  / ____/ __ \|  __ \|  ____|                          *
*                         | |__  | ' /  | |   | |  | | |__) | |__                             *
*                         |  __| |  <   | |   | |  | |  _  /|  __|                            *
*                         | |    | . \  | |___| |__| | | \ \| |____                           *
*                         |_|    |_|\_\  \_____\____/|_|  \_\______|                          *
*                                                                                             *
*                              Copyright (c) Frozen Kingdom WoW                               *
*                                                                                             *
*                                                                                             *
*            Diese Emulation unterliegt vollständig dem Copyright / Kopierschutz              *
*            von Frozen Kingdom WoW. Sämtliche Vervielfältigung oder Verbreitung              *
*            dieser hier enthaltenen Medien wird juristisch bestraft. Diese                   *
*            Software darf nur vom Herausgeber Frozen Kingdom WoW verwendet bzw               *
*            Weiterverwendet werden.                                                          *
*                                                                                             *
*                                                                                             *
*            This emulation is subject to the law of copyright from "The Frozen               *
*            Kingdom World of Warcraft" servers. All copying of these here contained          *
*            content, will be legitimate punished. This software must only used by            *
*            the publisher.                                                                   *
*                                                                                             *
**********************************************************************************************/

#include "ScriptPCH.h"
#include "karazhan.h"

enum NPCs
{
    TRIGGER_ID      = 22519,
    NPC_MEDIVH      = 16816,
    NPC_PAWN_H      = 17469,
    NPC_PAWN_A      = 17211,
    NPC_KNIGHT_H_S_E= 21748,
    NPC_KNIGHT_A    = 21664,
    NPC_QUEEN_H     = 21750,
    NPC_QUEEN_A     = 21683,
    NPC_BISHOP_H    = 21747,
    NPC_BISHOP_A    = 21682,
    BISHOP_HEAL_H   = 37456,
    BISHOP_HEAL_A   = 37455,
    NPC_ROOK_H      = 21726,
    NPC_ROOK_A      = 21160,
    NPC_KING_H      = 21752,
    NPC_KING_A      = 21684,
};
    
enum Spells
{
    SPELL_HAND_OF_MEDIVH   = 39339,
    SPELL_FURY_OF_MEDIVH   = 39383,
    SPELL_MOVE_1           = 37146,
    SPELL_MOVE_2           = 30012,
    SPELL_MOVE_3           = 37144,
    SPELL_MOVE_4           = 37148,
    SPELL_MOVE_5           = 37151,
    SPELL_MOVE_6           = 37152,
    SPELL_MOVE_7           = 37153,
    SPELL_CHANGE_FACING    = 30284,
    SPELL_MOVE_MARKER      = 32261,
};

enum Sonstiges
{
    ALLI_CREATURE_FACTION  = 1690,
    HORDE_CREATURE_FACTION = 1689,
    DUST_COVERED_CHEST     = 185119,
    NPC_ATTACK_RADIUS      = 7,
    SEARCH_RANGE           = 5,
    SOUND_START            = 10338,
    SOUND_KNIGHT_LOSE_P    = 10349,
    SOUND_KNIGHT_LOSE_M    = 10350,
    SOUND_LOSE_PAWN_P_1    = 10339,
    SOUND_LOSE_PAWN_P_2    = 10340,
    SOUND_LOSE_PAWN_P_3    = 10341,
    SOUND_LOSE_PAWN_M_1    = 10342,
    SOUND_LOSE_PAWN_M_2    = 10344,
    SOUND_LOSE_PAWN_M_3    = 10343,
    SOUND_LOSE_QUEEN_P     = 10351,
    SOUND_LOSE_QUEEN_M     = 10352,
    SOUND_LOSE_BISHOP_P    = 10347,
    SOUND_LOSE_BISHOP_M    = 10348,
    SOUND_LOSE_ROOK_P      = 10345,
    SOUND_LOSE_ROOK_M      = 10346,
    SOUND_PLAYER_CHECK     = 10353,
    SOUND_MEDIVH_CHECK     = 10354,
    SOUND_PLAYER_WIN       = 10355,
    SOUND_MEDIVH_WIN       = 10356,
    SOUND_MEDIVH_CHEAT_1   = 10357,    
    SOUND_MEDIVH_CHEAT_2   = 10358,
    SOUND_MEDIVH_CHEAT_3   = 10359,
};

enum Texts
{
	SAY_AT_EVENT_START,
    SAY_LOSE_KNIGHT_P,
    SAY_LOSE_KNIGHT_M,
	SAY_LOSE_PAWN_P_1,
	SAY_LOSE_PAWN_P_2,
	SAY_LOSE_PAWN_P_3,
	SAY_LOSE_PAWN_M_1,
	SAY_LOSE_PAWN_M_2,
	SAY_LOSE_PAWN_M_3,
	SAY_LOSE_QUEEN_P,
	SAY_LOSE_QUEEN_M,
	SAY_LOSE_BISHOP_P,
	SAY_LOSE_BISHOP_M,
	SAY_LOSE_ROOK_P,
	SAY_LOSE_ROOK_M,
	SAY_PLAYER_CHECK,
	SAY_MEDIVH_CHECK,
	SAY_PLAYER_WIN,
	SAY_MEDIVH_WIN,
	SAY_MEDIVH_CHEAT_1,
	SAY_MEDIVH_CHEAT_2,
	SAY_MEDIVH_CHEAT_3,
};

#define GOSSIP_POSSES "Ich übernehme!"
#define EVENT_START "Lasst uns Schach spielen!"

class chess_move_trigger : public CreatureScript
{
    public:
        chess_move_trigger() : CreatureScript("chess_move_trigger") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new chess_move_triggerAI(creature);
        }

        struct chess_move_triggerAI : public ScriptedAI
        { 
            chess_move_triggerAI(Creature* c) : ScriptedAI(c), _instance(c->GetInstanceScript())
            {
            }

            InstanceScript* _instance;
            uint32 search_time;
            Unit *unit_on_me;

            void Reset()
            {
                search_time = 4500;
                unit_on_me = NULL;
            }

            void Aggro(Unit *){}

            void UpdateAI(const uint32 diff)
            {
                if(_instance->GetData(TYPE_CHESS) != IN_PROGRESS)
                    return;
                    
                if(unit_on_me != NULL)
                {
                    if(search_time < diff )
                    {
                        if(me->GetDistance2d(unit_on_me) != 0 || unit_on_me->isAlive())
                        {
                            unit_on_me = NULL;
                        }
                        search_time = 500;
                    }
                    else search_time -= diff;
                }
            }

            void SpellHit(Unit *caster,const SpellEntry *spell)
            {
                if(spell->Id == SPELL_CHANGE_FACING)
                    caster->SetInFront(me);
                if(spell->Id == SPELL_MOVE_1 || spell->Id == SPELL_MOVE_2 || spell->Id == SPELL_MOVE_3
                    || spell->Id == SPELL_MOVE_4 || spell->Id == SPELL_MOVE_5 || spell->Id == SPELL_MOVE_6
                    || spell->Id == SPELL_MOVE_7)
                {
                    if(unit_on_me != NULL)
                        return;
                        
                    caster->GetMotionMaster()->Clear();
                    caster->GetMotionMaster()->MovePoint(0,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
                    unit_on_me = caster;
                    DoCast(me,SPELL_MOVE_MARKER);
                }
            }

        };
};

class chess_npc : public CreatureScript
{
    public:
        chess_npc() : CreatureScript("chess_npc") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new chess_npcAI(creature);
        }

        struct chess_npcAI : public ScriptedAI
        {
            chess_npcAI(Creature* c) : ScriptedAI(c) {}

            Creature *npc_medivh;
            bool sendedtohome;
            bool isInGame;
            bool canmove;
            uint32 original_faction_of_player;
            Unit *creaturecharmer;
            uint64 medivhGUID;
            InstanceScript* instance;
            uint32 healtimer;
            uint32 move_timer;
            std::list<Unit *> PossibleMoveUnits;
            uint64 playerGUID;
            uint64 ImageGUID;
  
            void MoveInLineOfSight(Unit *who)
            {
                if (me->getVictim() && who->isTargetableForAttack() && ( me->IsHostileTo( who )) && who->isInAccessiblePlaceFor(me) )
                {
                    if (!me->CanFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                        return;

                    if( me->IsWithinDistInMap(who, NPC_ATTACK_RADIUS) && me->IsWithinLOSInMap(who) && isInGame)
                    {
                        who->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                        AttackStart(who);
                        playerGUID = who->GetGUID();
                    }
                }
            }

            void Aggro(Unit *Unit)
            {
                ImageGUID = instance->GetData64(DATA_IMAGE_OF_MEDIVH);
                npc_medivh = (Creature*)Unit::GetUnit(*me,ImageGUID);
                Player* player = Unit::GetPlayer(*me, playerGUID);
                
                if(npc_medivh && player)
                {
                    if (player->GetTeam() == ALLIANCE)
                    {
                        if(me->GetEntry() == NPC_KING_H)
                        {
							npc_medivh->Say(SAY_MEDIVH_CHECK,LANG_UNIVERSAL,NULL);
                        }
                        else if(me->GetEntry() == NPC_KING_A)
                        {
                            npc_medivh->Say(SAY_PLAYER_CHECK,LANG_UNIVERSAL,NULL);
                        }
                    }
                    else
                    {
                        if(me->GetEntry() == NPC_KING_A)
                        {
                            npc_medivh->Say(SAY_MEDIVH_CHECK,LANG_UNIVERSAL,NULL);
                        }
                        else if(me->GetEntry() == NPC_KING_H)
                        {
                            npc_medivh->Say(SAY_PLAYER_CHECK,LANG_UNIVERSAL,NULL);
                        }
                    }
                }
            }

            void Reset()
            {
                healtimer = 7000;
                sendedtohome = false;
                isInGame = true;
                canmove = true;
                move_timer = 3000;
                me->setActive(true);
                playerGUID = 0;
            }
            
            void JustRespawned()
            {
                float angle = me->GetOrientation();
                float pos_x = -11066;
                float pos_y = -1898;
                int move_lenght = 2*rand()%10;
                float new_x = pos_x + move_lenght * cos(angle);
                float new_y = pos_y + move_lenght * sin(angle);
                me->Relocate(new_x,new_y,221,2.24);
                me->CombatStop();
                me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
            }
            
            void JustDied(Unit *Killer)
            {
                isInGame = false;
                me->setActive(false);
                me->Respawn();
            }

            void OnPossess(bool Apply)
            {
                if(Apply)
                {
                    Player* player;
                    original_faction_of_player = me->GetCharmer()->getFaction();
                    creaturecharmer = me->GetCharmer();
                    if ((player = me->GetPlayer(*me, playerGUID)))
                        return;
                        
                    if(player->GetTeam() == ALLIANCE)
                    {
                        me->setFaction(ALLI_CREATURE_FACTION);
                        creaturecharmer->setFaction(ALLI_CREATURE_FACTION);
                    }
                    else
                    {
                        me->setFaction(HORDE_CREATURE_FACTION);
                        creaturecharmer->setFaction(HORDE_CREATURE_FACTION);
                    }
                }
                else
                {
                    creaturecharmer->setFaction(original_faction_of_player);
                }

            }

            void KilledUnit(Unit* victim)
            {
                ImageGUID = instance->GetData64(DATA_IMAGE_OF_MEDIVH);
                npc_medivh = (Creature*)Unit::GetUnit(*me,ImageGUID);
                Player* player = Unit::GetPlayer(*me, playerGUID);
                if(!npc_medivh || !player)
                    return;

                if(player->GetTeam() == HORDE)
                {
                    switch(victim->GetEntry())
                    {
                        case NPC_ROOK_H: npc_medivh->Say(SAY_LOSE_ROOK_P,LANG_UNIVERSAL,NULL);break;
                        case NPC_ROOK_A: npc_medivh->Say(SAY_LOSE_ROOK_M,LANG_UNIVERSAL,NULL);break;
                        case NPC_QUEEN_H: npc_medivh->Say(SAY_LOSE_QUEEN_P,LANG_UNIVERSAL,NULL);break;
                        case NPC_QUEEN_A: npc_medivh->Say(SAY_LOSE_QUEEN_M,LANG_UNIVERSAL,NULL);break;
                        case NPC_BISHOP_H: npc_medivh->Say(SAY_LOSE_BISHOP_P,LANG_UNIVERSAL,NULL);break;
                        case NPC_BISHOP_A: npc_medivh->Say(SAY_LOSE_BISHOP_M,LANG_UNIVERSAL,NULL);break;
                        case NPC_KNIGHT_H_S_E: npc_medivh->Say(SAY_LOSE_KNIGHT_P,LANG_UNIVERSAL,NULL);break;
                        case NPC_KNIGHT_A: npc_medivh->Say(SAY_LOSE_KNIGHT_M,LANG_UNIVERSAL,NULL);break;
                        case NPC_PAWN_H:
                            switch(rand()%3)
                            {
                                case 0:npc_medivh->Say(SAY_LOSE_PAWN_P_1,LANG_UNIVERSAL,NULL);break;
                                case 1:npc_medivh->Say(SAY_LOSE_PAWN_P_2,LANG_UNIVERSAL,NULL);break;
                                case 2:npc_medivh->Say(SAY_LOSE_PAWN_P_3,LANG_UNIVERSAL,NULL);break;
                            }
                            break;
                        case NPC_PAWN_A:
                            switch(rand()%2)
                            {
                                case 0:npc_medivh->Say(SAY_LOSE_PAWN_M_1,LANG_UNIVERSAL,NULL);break;
                                case 1:npc_medivh->Say(SAY_LOSE_PAWN_M_2,LANG_UNIVERSAL,NULL);break;
                            }
                            break;
                        case NPC_KING_H:
							//              npc_medivh->Say(SAY_MEDIVH_WIN,LANG_UNIVERSAL,NULL);
							instance->SetData(TYPE_CHESS,FAIL);
							break;
 
						/*case NPC_KING_H:
							 //              npc_medivh->Say(SAY_MEDIVH_WIN,LANG_UNIVERSAL,NULL);
							 instance->SetData(TYPE_CHESS,FAIL);
							  break;*/
						 case NPC_KING_A:
							 //      npc_medivh->Say(SAY_PLAYER_WIN,LANG_UNIVERSAL,NULL);
							instance->SetData(TYPE_CHESS,DONE);
							npc_medivh->SummonGameObject(DUST_COVERED_CHEST,-11058,-1903,221,2.24,0,0,0,0,7200000);
							break;
						 default:
							break;
                    }
                }
                else
                {
                    switch(victim->GetEntry())
                    {
                        case NPC_ROOK_A: npc_medivh->Say(SAY_LOSE_ROOK_P,LANG_UNIVERSAL,NULL);break;
                        case NPC_ROOK_H: npc_medivh->Say(SAY_LOSE_ROOK_M,LANG_UNIVERSAL,NULL);break;
                        case NPC_QUEEN_A: npc_medivh->Say(SAY_LOSE_QUEEN_P,LANG_UNIVERSAL,NULL);break;
                        case NPC_QUEEN_H: npc_medivh->Say(SAY_LOSE_QUEEN_M,LANG_UNIVERSAL,NULL);break;
                        case NPC_BISHOP_A: npc_medivh->Say(SAY_LOSE_BISHOP_P,LANG_UNIVERSAL,NULL);break;
                        case NPC_BISHOP_H: npc_medivh->Say(SAY_LOSE_BISHOP_M,LANG_UNIVERSAL,NULL);break;
                        case NPC_KNIGHT_A: npc_medivh->Say(SAY_LOSE_KNIGHT_P,LANG_UNIVERSAL,NULL);break;
                        case NPC_KNIGHT_H_S_E: npc_medivh->Say(SAY_LOSE_KNIGHT_M,LANG_UNIVERSAL,NULL);break;
                        case NPC_PAWN_A:
                            switch(rand()%3)
                            {
                                case 0:npc_medivh->Say(SAY_LOSE_PAWN_P_1,LANG_UNIVERSAL,NULL);break;
                                case 1:npc_medivh->Say(SAY_LOSE_PAWN_P_2,LANG_UNIVERSAL,NULL);break;
                                case 2:npc_medivh->Say(SAY_LOSE_PAWN_P_3,LANG_UNIVERSAL,NULL);break;
                            }
                        case NPC_PAWN_H:
                            switch(rand()%2)
                            {
                                case 0:npc_medivh->Say(SAY_LOSE_PAWN_M_1,LANG_UNIVERSAL,NULL);break;
                                case 1:npc_medivh->Say(SAY_LOSE_PAWN_M_2,LANG_UNIVERSAL,NULL);break;
                            }
                            break;
                        case NPC_KING_A:
                            npc_medivh->Say(SAY_MEDIVH_WIN,LANG_UNIVERSAL,NULL);
                            instance->SetData(TYPE_CHESS,FAIL);
                            break;
                        case NPC_KING_H:
                            npc_medivh->Say(SAY_PLAYER_WIN,LANG_UNIVERSAL,NULL);
                            instance->SetData(TYPE_CHESS,DONE);
                            npc_medivh->SummonGameObject(DUST_COVERED_CHEST,-11058,-1903,221,2.24,0,0,0,0,7200000);
                            break;
                        default:
                            break;
                    }
                }
                if(victim->isPossessed())
                {
                    victim->RemoveCharmAuras();
                }

            }

            std::list<Unit*> FindPossibleMoveUnits()
            {
                /*GridCoord p(Trinity::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
                Cell cell(p);
                cell.data.Part.reserved = ALL_DISTRICT;
                cell.SetNoCreate();*/
                std::list<Unit*> pList;
                /*std::list<Unit*> returnList;
                float range = GetStrafeLenght(me->GetEntry())*SEARCH_RANGE;

                Trinity::AnyUnitInObjectRangeCheck u_check(me,range);
                Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher(pList, u_check);
                TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

                CellCoord<GridReadGuard> cell_lock(cell, p);
                cell_lock->Visit(cell_lock, grid_unit_searcher, *(me->GetMap()));
                for(std::list<Unit *>::iterator itr = pList.begin(); itr != pList.end();itr++)
                {
                    if((*itr)->GetEntry() != TRIGGER_ID)
                    {
                        continue;
                    }
                    returnList.push_back((*itr));
                    pList.clear();
                    return returnList;
                }*/
                return pList;
            }

            int GetStrafeLenght(uint32 Entry)
            {
                if(Entry == NPC_QUEEN_H ||Entry == NPC_QUEEN_A)
                    return 2;
                return 1;
            }

            void UpdateAI(const uint32 diff)
            {
                if(instance->GetData(DATA_CHESS_EVENT) == DONE || instance->GetData(DATA_CHESS_EVENT) == FAIL)
                {
                    if(me->isInCombat())
                        me->CombatStop();

                    if(me->isPossessed())
                        me->RemoveCharmAuras();

                    if(me->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE))
                        me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);

                    if(!sendedtohome)
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveTargetedHome();
                        sendedtohome = true;
                    }
                }
                if(instance->GetData(DATA_CHESS_EVENT) != IN_PROGRESS)
                    return;

                if(sendedtohome)
                {
                    sendedtohome = false;
                }
                
                if(!isInGame)
                    return;

                if(!me->isPossessed())
                {
                    if(!canmove)
                    {
                        if(move_timer < diff)
                        {
                            canmove = true;
                            move_timer = 6000;
                        }
                        else  move_timer -= diff;
                    }

                    if(!me->getVictim())
                    {
                        if(!canmove)
                            return;

                        PossibleMoveUnits = FindPossibleMoveUnits();
                        if(PossibleMoveUnits.empty())
                            return;
                        std::list<Unit*>::iterator i = PossibleMoveUnits.begin();
                        advance ( i ,rand()%PossibleMoveUnits.size());
                        DoCast((*i),SPELL_MOVE_1);
                        canmove = false;
                    }

                    switch(me->GetEntry())
                    {
                        case NPC_ROOK_A:break;
                        case NPC_ROOK_H:break;
                        case NPC_QUEEN_A:break;
                        case NPC_QUEEN_H:break;
                        case NPC_BISHOP_A:
                            if(healtimer <= diff)
                            {
                                Unit *target = DoSelectLowestHpFriendly(25,5000);
                                if(target)
                                    DoCast(target,BISHOP_HEAL_A);
                                healtimer = 21000;
                            }
                            else healtimer -= diff;
                                break;
                        case NPC_BISHOP_H:
                            if(healtimer <= diff)
                            {
                                Unit *target = DoSelectLowestHpFriendly(25,5000);
                                if(target)
                                    DoCast(target,BISHOP_HEAL_H);
                                healtimer = 21000;
                            }
                            else healtimer -= diff;
                                break;
                        case NPC_KNIGHT_A:
                            break;
                        case NPC_KNIGHT_H_S_E:
                            break;
                        case NPC_PAWN_A:
                            break;
                        case NPC_PAWN_H:
                            break;
                        case NPC_KING_A:
                            break;
                        case NPC_KING_H:
                            break;
                        default:
                            break;
                    }
                    if(me->getVictim() || !me->IsWithinDistInMap(me->getVictim(), NPC_ATTACK_RADIUS))
                    {
                        me->CombatStop();
                        return;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };
};

bool OnGossipHello(Player* player, Creature* _Creature)
{
    InstanceScript* instance = ((InstanceScript*)_Creature->GetInstanceScript());

    if(instance->GetData(TYPE_CHESS) != IN_PROGRESS)
        return true;
    if(player->GetTeam() == ALLIANCE && _Creature->getFaction() != ALLI_CREATURE_FACTION)
        return true;
    if(player->GetTeam() == HORDE && _Creature->getFaction() != HORDE_CREATURE_FACTION)
        return true;
    if(!_Creature->isPossessedByPlayer())
        player->ADD_GOSSIP_ITEM(0, GOSSIP_POSSES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->SEND_GOSSIP_MENU(8990, _Creature->GetGUID());
    return true;
}

bool OnGossipSelect(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    if(action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        _Creature->SetCharmedBy(player, CHARM_TYPE_POSSESS);
        player->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_DISABLE_MOVE);
        _Creature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_DISABLE_MOVE);
    }
    player->CLOSE_GOSSIP_MENU();
    return true;
}

class npc_echo_of_medivh : public CreatureScript
{
    public:
        npc_echo_of_medivh() : CreatureScript("npc_echo_of_medivh") { }

        bool OnGossipHello(Player* player, Creature* _Creature)
        {
            InstanceScript* instance = ((InstanceScript*)_Creature->GetInstanceScript());
            if(instance->GetData(TYPE_CHESS) == FAIL)
            {
                instance->SetData(TYPE_CHESS,NOT_STARTED);
            }
            if(instance->GetData(TYPE_CHESS) == DONE)
            {
                instance->SetData(TYPE_CHESS,NOT_STARTED);
            }

            if(instance->GetData(TYPE_CHESS) != NOT_STARTED)
                return true;

            player->ADD_GOSSIP_ITEM(0, EVENT_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(8990, _Creature->GetGUID());
            //_Creature->GetNpcTextId()
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* _Creature, uint32 sender, uint32 action)
        {
            InstanceScript* instance = ((InstanceScript*)_Creature->GetInstanceScript());

            if(action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                //_Creature->Say(SAY_AT_EVENT_START,LANG_UNIVERSAL,NULL);
                instance->SetData(TYPE_CHESS,IN_PROGRESS);
                //instance->SetData(CHESS_EVENT_TEAM,player->GetTeam());
                _Creature->GetMotionMaster()->MoveConfused();
                //player->TeleportTo(-11054.032,-1909.979,229.626,2.190);
            }
            player->CLOSE_GOSSIP_MENU();
            return true;
        }
};

void AddSC_chess_event()
{
    new chess_npc();
    new npc_echo_of_medivh();
    new chess_move_trigger();
}