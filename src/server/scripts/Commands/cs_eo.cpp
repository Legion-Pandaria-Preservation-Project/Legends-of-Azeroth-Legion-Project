/*
 *###############################################################################
 *#                                                                             #
 *# Copyright (C) 2022 Project Nighthold <https://github.com/ProjectNighthold>  #
 *#                                                                             #
 *# This file is free software; as a special exception the author gives         #
 *# unlimited permission to copy and/or distribute it, with or without          #
 *# modifications, as long as this notice is preserved.                         #
 *#                                                                             #
 *# This program is distributed in the hope that it will be useful, but         #
 *# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the      #
 *# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    #
 *#                                                                             #
 *# Read the THANKS file on the source root directory for more info.            #
 *#                                                                             #
 *###############################################################################
 */

/* ScriptData
Name: eo_commandscript
%Complete: 100
Comment: All npc related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "EventObjectData.h"
#include "DatabaseEnv.h"

class eo_commandscript : public CommandScript
{
public:
    eo_commandscript() : CommandScript("eo_commandscript") { }

    ChatCommand* GetCommands() const override
    {
        static ChatCommand eoCommandTable[] =
        {
            { "near",           SEC_GAMEMASTER,     false, &HandleEONearCommand,               "", NULL },
            { "add",            SEC_GAMEMASTER,     false, &HandleEOAddCommand,                "", NULL },
            { "delete",         SEC_GAMEMASTER,     false, &HandleEODeleteCommand,             "", NULL },
            { NULL,             0,                  false, NULL,                               "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "eventobject",    SEC_MODERATOR,      false, NULL,                    "",  eoCommandTable },
            { NULL,             0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    //add spawn of creature
    static bool HandleEOAddCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* eoID = strtok((char*)args, " ");
        if (!eoID)
            return false;

        char* radiusStr = strtok(NULL, " ");
        char* spellStr = strtok(NULL, " ");
        char* worldsafeStr = strtok(NULL, " ");

        uint32 id  = atoi(eoID);
        float radius  = radiusStr ? atof(radiusStr) : 1.0f;
        uint32 spell  = spellStr ? atoi(spellStr) : 0;
        uint32 worldsafe  = worldsafeStr ? atoi(worldsafeStr) : 0;

        Player* chr = handler->GetSession()->GetPlayer();
        float x = chr->GetPositionX();
        float y = chr->GetPositionY();
        float z = chr->GetPositionZ();
        float o = chr->GetOrientation();
        Map* map = chr->GetMap();

        if (chr->GetTransport())
        {
            handler->SendSysMessage("Error, you on transport");
            return true;
        }

        EventObject* eventobject = new EventObject();
        if (!eventobject->Create(sObjectMgr->GetGenerator<HighGuid::EventObject>()->Generate(), map, chr->GetPhaseMgr().GetPhaseMaskForSpawn(), id, x, y, z, o, radius, spell, worldsafe))
        {
            delete eventobject;
            return false;
        }

        eventobject->SaveToDB(map->GetId(), (UI64LIT(1) << map->GetSpawnMode()), chr->GetPhaseMgr().GetPhaseMaskForSpawn());

        uint32 db_guid = eventobject->GetDBTableGUIDLow();

        delete eventobject;
        eventobject = new EventObject();
        if (!eventobject->LoadEventObjectFromDB(db_guid, map))
        {
            delete eventobject;
            return false;
        }

        sObjectMgr->AddEventObjectToGrid(db_guid, sEventObjectDataStore->GetEventObjectData(db_guid));
        return true;
    }

    static bool HandleEODeleteCommand(ChatHandler* handler, const char* args)
    {
        Creature* unit = NULL;

        if (*args)
        {
            char* eoID = strtok((char*)args, " ");
            if (!eoID)
            {
                handler->SendSysMessage("Error, need low guid");
                return false;
            }

            uint32 lowguid = strtoull(eoID, nullptr, 10);
            if (!lowguid)
            {
                handler->SendSysMessage("Error, need low guid");
                return false;
            }

            WorldDatabase.PExecute("DELETE FROM eventobject WHERE guid = %u", lowguid);
        }
        else
            handler->SendSysMessage("Error, need low guid");

        return true;
    }

    static bool HandleEONearCommand(ChatHandler* handler, char const* args)
    {
        float distance = (!*args) ? 10.0f : (float)(atof(args));
        uint32 count = 0;

        Player* player = handler->GetSession()->GetPlayer();

        PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_EVENTOBJECT_NEAREST);
        stmt->setFloat(0, player->GetPositionX());
        stmt->setFloat(1, player->GetPositionY());
        stmt->setFloat(2, player->GetPositionZ());
        stmt->setUInt32(3, player->GetMapId());
        stmt->setFloat(4, player->GetPositionX());
        stmt->setFloat(5, player->GetPositionY());
        stmt->setFloat(6, player->GetPositionZ());
        stmt->setFloat(7, distance * distance);
        PreparedQueryResult result = WorldDatabase.Query(stmt);

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 guid = fields[0].GetUInt64();
                uint32 entry = fields[1].GetUInt32();
                float x = fields[2].GetFloat();
                float y = fields[3].GetFloat();
                float z = fields[4].GetFloat();
                uint16 mapId = fields[5].GetUInt16();

                EventObjectTemplate const* eoInfo = sEventObjectDataStore->GetEventObjectTemplate(entry);
                if (!eoInfo)
                    continue;

                handler->PSendSysMessage(LANG_NPC_LIST_CHAT, guid, entry, guid, eoInfo->Name.c_str(), x, y, z, mapId);

                ++count;
            } while (result->NextRow());
        }

        handler->PSendSysMessage(LANG_COMMAND_NEAROBJMESSAGE, distance, count);
        return true;
    }
};

void AddSC_eo_commandscript()
{
    new eo_commandscript();
}