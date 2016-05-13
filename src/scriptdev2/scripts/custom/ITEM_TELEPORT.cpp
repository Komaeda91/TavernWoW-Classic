#include "precompiled.h"
#include "Spell.h"
#include "Common.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "WorldSession.h"
#include "Object.h"
#include "Chat.h"
#include "SpellMgr.h"
#include "DBCStructure.h"
#include "D:\CmangosBuild\mangos-classic\src\game\BattleGround\BattleGroundMgr.h"
#include "D:\CmangosBuild\mangos-classic\src\game\BattleGround\BattleGroundWS.h"
#include "D:\CmangosBuild\mangos-classic\src\game\BattleGround\BattleGround.h"
#include "Language.h"
#include "ScriptMgr.h"
#include "World.h"
#include "Chat.h"

#pragma comment(lib,"ws2_32.lib")

#if _MSC_VER >= 1600 // VC2010
#pragma execution_character_set("utf-8")
#endif


static char _StringConversionStorage[2048];
const char* _ToUTF8(const char *strGBK)
{
	//static char _StringConversionStorage[2048];
	DWORD     UniCodeLen = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, 0, 0);
	std::vector <wchar_t>   vWCH(UniCodeLen);
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, &vWCH[0], UniCodeLen);
	DWORD   dwUtf8Len = WideCharToMultiByte(CP_UTF8, 0, &vWCH[0], UniCodeLen, NULL, NULL, NULL, NULL);
	if (dwUtf8Len >= 2048) dwUtf8Len = 2047;
	memset(_StringConversionStorage, 0, (sizeof(char)*dwUtf8Len) + 1);
	WideCharToMultiByte(CP_UTF8, 0, &vWCH[0], UniCodeLen, _StringConversionStorage, dwUtf8Len, NULL, NULL);
	return &_StringConversionStorage[0];
}

bool ItemUse_frozen_transform(Player* player, Item* _Item, SpellCastTargets const& targets)
{
	player->Say("1",LANG_UNIVERSAL);
	uint8 slot = NULL_SLOT;
	player->scriptslot = NULL_SLOT;
	player->scriptdisplayEntry = 0;
	if (Item * item = targets.getItemTarget())
	{
		if (!item->GetProto())
		{
			player->GetSession()->SendNotification(_ToUTF8("��Ʒ����1"));
			return true;
		}
		switch (item->GetProto()->InventoryType)
		{
		case INVTYPE_HEAD:
			slot = EQUIPMENT_SLOT_HEAD;
			break;
		case INVTYPE_SHOULDERS:
			slot = EQUIPMENT_SLOT_SHOULDERS;
			break;
		case INVTYPE_CHEST:
		case INVTYPE_ROBE:
			slot = EQUIPMENT_SLOT_CHEST;
			break;
		case INVTYPE_WAIST:
			slot = EQUIPMENT_SLOT_WAIST;
			break;
		case INVTYPE_LEGS:
			slot = EQUIPMENT_SLOT_LEGS;
			break;
		case INVTYPE_FEET:
			slot = EQUIPMENT_SLOT_FEET;
			break;
		case INVTYPE_WRISTS:
			slot = EQUIPMENT_SLOT_WRISTS;
			break;
		case INVTYPE_HANDS:
			slot = EQUIPMENT_SLOT_HANDS;
			break;
		case INVTYPE_CLOAK:
			slot = EQUIPMENT_SLOT_BACK;
			break;
		case INVTYPE_WEAPON:
		case INVTYPE_2HWEAPON:
		case INVTYPE_WEAPONMAINHAND:
			slot = EQUIPMENT_SLOT_MAINHAND;
			break;
		case INVTYPE_SHIELD:
		case INVTYPE_WEAPONOFFHAND:
		case INVTYPE_HOLDABLE:
			slot = EQUIPMENT_SLOT_OFFHAND;
			break;
		case INVTYPE_RANGED:
		case INVTYPE_THROWN:
		case INVTYPE_RANGEDRIGHT:
		case INVTYPE_RELIC:
			slot = EQUIPMENT_SLOT_RANGED;
			break;
		}
		if (Item * titem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
		{
			if (!titem->GetProto())
			{
				player->GetSession()->SendNotification(_ToUTF8("��Ʒ����2"));
				return true;
			}

			if (titem->GetProto()->Class != item->GetProto()->Class || titem->GetProto()->SubClass != item->GetProto()->SubClass)
			{
				player->GetSession()->SendNotification(_ToUTF8("������û��װ��������Ʒ."));
				return true;
			}
			std::string text = _ToUTF8("����˴���");
			switch (slot)
			{
			case EQUIPMENT_SLOT_HEAD:
				text += _ToUTF8(_ToUTF8("ͷ����"));
				break;
			case EQUIPMENT_SLOT_SHOULDERS:
				text += _ToUTF8("�粿��");
				break;
			case EQUIPMENT_SLOT_CHEST:
				text += _ToUTF8("�ز���");
				break;
			case EQUIPMENT_SLOT_WAIST:
				text += _ToUTF8("������");
				break;
			case EQUIPMENT_SLOT_LEGS:
				text += _ToUTF8("�Ȳ���");
				break;
			case EQUIPMENT_SLOT_FEET:
				text += _ToUTF8("�Ų���");
				break;
			case EQUIPMENT_SLOT_WRISTS:
				text += _ToUTF8("�����");
				break;
			case EQUIPMENT_SLOT_HANDS:
				text += _ToUTF8("�ֲ���");
				break;
			case EQUIPMENT_SLOT_BACK:
				text += _ToUTF8("������");
				break;
			case EQUIPMENT_SLOT_MAINHAND:
				text += _ToUTF8("���ֵ�");
				break;
			case EQUIPMENT_SLOT_OFFHAND:
				text += _ToUTF8("���ֵ�");
				break;
			case EQUIPMENT_SLOT_RANGED:
				text += _ToUTF8("Զ�̵�");
				break;
			}

			text += "[";
			text += titem->GetProto()->Name1;
			text += _ToUTF8("]�û�Ϊ��[");
			text += item->GetProto()->Name1;
			text += "]";
			player->scriptslot = slot;
			player->scriptdisplayEntry = item->GetEntry();
			player->ADD_GOSSIP_ITEM(10, text.c_str(), 1, GOSSIP_ACTION_INFO_DEF);
		}
		else
			player->GetSession()->SendNotification(_ToUTF8("������û��װ��������Ʒ."));
	}
	else
	{
		player->GetSession()->SendNotification(_ToUTF8("��Ʒ����0"));
		return true;
	}
	player->ADD_GOSSIP_ITEM(10, _ToUTF8("�ر�"), 1, GOSSIP_ACTION_INFO_DEF + 1);
	player->PlayerTalkClass->SendGossipMenu(822, _Item->GetGUID());
	return true;
}

bool ItemSelect_frozen_transform(Player *player, Item *pItem, uint32 sender, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF:
	{
		if (Item * item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, player->scriptslot))
		{
		  printf(" slot =%u   displayEntry = %u\n", player->scriptslot, player->scriptdisplayEntry);
		  item->m_DisplayEntry = player->scriptdisplayEntry;

		  player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + (player->scriptslot * MAX_VISIBLE_ITEM_OFFSET), player->scriptdisplayEntry);
		  item->SetState(ItemUpdateState::ITEM_CHANGED, player);
		  item->SaveToDB();
		}
		uint32 count = 1;
		player->DestroyItemCount(pItem, count, true);
		player->CLOSE_GOSSIP_MENU();
	}break;
	case GOSSIP_ACTION_INFO_DEF + 1:
	{
									   player->CLOSE_GOSSIP_MENU();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
	}break;
	}
	return true;
}

bool ItemUse_frozen_detransform(Player* player, Item* pItem, SpellCastTargets const& scTargets)
{
	if (Item * item = scTargets.getItemTarget())
	{
		player->scriptitemguid = item->GetGUID();
		std::string text = _ToUTF8("����˴���[");
		text += item->GetProto()->Name1;
		text += _ToUTF8("]�Ļû�Ч�������");
		player->ADD_GOSSIP_ITEM(10, text.c_str(), 1, GOSSIP_ACTION_INFO_DEF + 1);
	}
	player->ADD_GOSSIP_ITEM(10, _ToUTF8("�ر�"), 1, GOSSIP_ACTION_INFO_DEF + 1);
	player->PlayerTalkClass->SendGossipMenu(822, pItem->GetGUID());
	return true;
}

bool ItemSelect_frozen_detransform(Player *player, Item *pItem, uint32 sender, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF:
	{
		if (Item * item = player->GetItemByGuid(player->scriptitemguid))
		{
		   item->m_DisplayEntry = 0;
		   if (item->GetBagSlot() == INVENTORY_SLOT_BAG_0 && item->GetSlot() < EQUIPMENT_SLOT_END)
			   player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + (item->GetSlot() * MAX_VISIBLE_ITEM_OFFSET), item->GetEntry());
		}
		uint32 count = 1;
		player->DestroyItemCount(pItem, count, true);
		player->CLOSE_GOSSIP_MENU();
	}break;
	case GOSSIP_ACTION_INFO_DEF + 1:
	{
									   player->CLOSE_GOSSIP_MENU();
	}break;
	}
	return true;
}

bool ItemUse_Item_TelePort(Player* player, Item* _Item, SpellCastTargets const& scTargets)
{
	player->ADD_GOSSIP_ITEM(10, "����˲��", 1, GOSSIP_ACTION_INFO_DEF + 1);
	player->ADD_GOSSIP_ITEM(10, "����˫�츳", 1, GOSSIP_ACTION_INFO_DEF + 2);
	player->ADD_GOSSIP_ITEM(10, "�л�˫�츳", 1, GOSSIP_ACTION_INFO_DEF + 8);
	if (player->getLevel() < 60)
	{
		player->ADD_GOSSIP_ITEM(10, "��������", 1, GOSSIP_ACTION_INFO_DEF + 3);
	}
	player->ADD_GOSSIP_ITEM(10, "������ҵ����", 1, GOSSIP_ACTION_INFO_DEF + 5);
	player->ADD_GOSSIP_ITEM(10, "������ҵ����", 1, GOSSIP_ACTION_INFO_DEF + 6);
	player->ADD_GOSSIP_ITEM(10, "���򱳰�", 1, GOSSIP_ACTION_INFO_DEF + 7);
	player->ADD_GOSSIP_ITEM(10, "����ϣ����", 1, GOSSIP_ACTION_INFO_DEF + 9);
	player->ADD_GOSSIP_ITEM(10, "ս�����", 1, GOSSIP_ACTION_INFO_DEF + 10);
	player->ADD_GOSSIP_ITEM(10, "��ɽ����", 1, GOSSIP_ACTION_INFO_DEF + 11);
	player->SEND_GOSSIP_MENU(822,_Item->GetGUID());
	return true;
}
bool ItemSelect_Item_TelePort(Player *pPlayer, Item *pItem, uint32 sender, uint32 action)
{
	uint32 jf;
	uint32 skillcount = 0;
	uint32 maxskilljf = 0;
	uint32 learnskilljf;
	uint32 bagjf;
	uint32 doubletalentjf;
	uint32 sfjf;
	uint32 levelupjf;
	uint32 maxlevelupjf;
	uint32 maxskillcount;
	uint32 oldlevel;
	uint32 uplevel;
	auto jfresult = pPlayer->PQuery(GameDB::RealmDB, "SELECT jf FROM account WHERE id = %u", pPlayer->GetSession()->GetAccountId());
	if (jfresult)
	{
		auto field = jfresult->Fetch();
		jf = field[0].GetUInt32();
	}													//			0				1		2		3		4				5				
	auto needjfresult = pPlayer->PQuery(GameDB::WorldDB, "SELECT modifyskilljf,learnskilljf,bagjf,levelupjf,maxlevelupjf,maxskillcount FROM world_conf");
	if (needjfresult)
	{
		auto field = needjfresult->Fetch();
		maxskilljf = field[0].GetUInt32();
		learnskilljf = field[1].GetUInt32();
		bagjf = field[2].GetUInt32();
		levelupjf = field[3].GetUInt32();
		maxlevelupjf = field[4].GetUInt32();
		maxskillcount = field[5].GetUInt32();
	}
	if (pPlayer->HasSpell(2575))
	{
		skillcount++;
	}
	if (pPlayer->HasSpell(2259))
	{
		skillcount++;
	}
	if (pPlayer->HasSpell(2018))
	{
		skillcount++;
	}
	if (pPlayer->HasSpell(3908))
	{
		skillcount++;
	}
	if (pPlayer->HasSpell(7414))
	{
		skillcount++;
	}
	if (pPlayer->HasSpell(4036))
	{
		skillcount++;
	}
	if (pPlayer->HasSpell(2372))
	{
		skillcount++;
	}
	if (pPlayer->HasSpell(8613))
	{
		skillcount++;
	}
	if (pPlayer->HasSpell(2108))
	{
		skillcount++;
	}
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF + 9:
	{
									   pPlayer->GetSession()->SendBattlegGroundList(17379391213815256417, BATTLEGROUND_AB); //����ϣ ����
									   return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 10:
	{
									   pPlayer->GetSession()->SendBattlegGroundList(17379391213362271574, BATTLEGROUND_WS); //ս�� ����
									   return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 11:
	{
									   pPlayer->GetSession()->SendBattlegGroundList(17379391086341957517, BATTLEGROUND_AV); //��ɽ ����
									   return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 8:
	{
									   //Creature* pCreature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
									 //Unit*player = pPlayer;
									 //uint32 menuid;
									 //pPlayer->PrepareGossipMenu(pPlayer, 6504);
									 //pPlayer->SendPreparedGossip(pPlayer);
									 //pPlayer->GetSession()->SendBattlegGroundList(17379391213529989638, BATTLEGROUND_AB); //����ϣ ����
									 //pPlayer->GetSession()->SendBattlegGroundList(17379391213362271574, BATTLEGROUND_WS); //ս�� ����
									 //pPlayer->GetSession()->SendBattlegGroundList(17379391086341957517, BATTLEGROUND_AV); //��ɽ ����

									// pPlayer->SEND_GOSSIP_MENU(6504, pItem->GetGUID());
		ChatHandler(pPlayer).ParseCommands(".tf 1save");
		return true;
		/*if (pPlayer->CanDoubleTalent == false)
		{
			pPlayer->GetSession()->SendNotification("��û��Ȩ�ޣ�");
			return true;
		}
		else
		{
			if (pPlayer->isInCombat())
			{
				ChatHandler(pPlayer).PSendSysMessage("ս�����޷��л��츳��");
				return true;
			}
			if (!pPlayer->CanDoubleTalent)
			{
				ChatHandler(pPlayer).PSendSysMessage("��û�л��˫�츳Ȩ�ޣ�");
				return true;
			}
			std::vector<PlayerTalentSpell> bak_talent;
			auto result = pPlayer->PQuery(GameDB::CharactersDB,"SELECT guid,spell,active,disabled,free FROM character_spell_talent WHERE guid=%u", pPlayer->GetGUIDLow()); //ƥ�䱸���츳
			if (result)
			{
				do
				{
					Field* field = result->Fetch();            //���汸���츳
					uint32 guid = field[0].GetUInt32();
					uint32 spell = field[1].GetUInt32();
					uint32 active = field[2].GetUInt32();
					uint32 disabled = field[3].GetUInt32();
					uint32 freepoint = field[4].GetUInt32();
					PlayerTalentSpell tmp_talent;
					tmp_talent.guid = guid;
					tmp_talent.spell = spell;
					tmp_talent.active = active;
					tmp_talent.disabled = disabled;
					tmp_talent.freepoint = freepoint;
					bak_talent.push_back(tmp_talent);
				} while (result->NextRow());
				pPlayer->PExecute(GameDB::CharactersDB, "DELETE FROM character_spell_talent WHERE guid=%u", pPlayer->GetGUIDLow()); //ɾ�������츳
			}
			for (PlayerSpellMap::const_iterator itr = pPlayer->GetSpellMap().begin(); itr != pPlayer->GetSpellMap().end(); ++itr) //
			{
				if (pPlayer->HasSpell(itr->first))
					pPlayer->PExecute(GameDB::CharactersDB, "INSERT INTO character_spell_talent(guid,spell,active,disabled,free) VALUES (%u,%u,%u,%u,%u)", pPlayer->GetGUIDLow(), itr->first, 1, !IsPassiveSpell(itr->first), pPlayer->GetFreeTalentPoints());
			}
			pPlayer->SaveToDB();
			pPlayer->GetSession()->SendNotification("��һ���츳����ɹ���");
			for (unsigned int i = 0; i < sTalentStore.GetNumRows(); ++i)
			{
				TalentEntry const* talentInfo = sTalentStore.LookupEntry(i);
				if (!talentInfo)
					continue;
				TalentTabEntry const* talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
				if (!talentTabInfo)
					continue;
				// unlearn only talents for character class
				// some spell learned by one class as normal spells or know at creation but another class learn it as talent,
				// to prevent unexpected lost normal learned spell skip another class talents
				if ((pPlayer->getClassMask() & talentTabInfo->ClassMask) == 0)
					continue;
	
				for (int j = 4; j >= 0; --j)
				if (talentInfo->RankID[j])
				{
					if (pPlayer->HasSpell(talentInfo->RankID[j]))
					{
						pPlayer->removeSpell(talentInfo->RankID[j], false);
					}
				}
			}
			uint32 freepoint = 0;
			for each (PlayerTalentSpell var in bak_talent)
			{
				if (!pPlayer->HasSpell(var.spell))
				{
					pPlayer->learnSpell(var.spell, false);
				}
				freepoint = var.freepoint;
			}
			pPlayer->SetFreeTalentPoints(freepoint);
			ChatHandler(pPlayer).PSendSysMessage("�츳�л��ɹ���");
			pPlayer->SaveToDB();
			return true;
		}*/
	}
	case GOSSIP_ACTION_INFO_DEF + 7: //����
	{
		ChatHandler(pPlayer).PSendSysMessage("������Ҫ����%u����֣���ȷ�ϣ�", bagjf);
		pPlayer->ADD_GOSSIP_ITEM(10, "ȷ�Ϲ���", 1, GOSSIP_ACTION_INFO_DEF + 70);
		pPlayer->ADD_GOSSIP_ITEM(10, "ȡ��", 1, GOSSIP_ACTION_INFO_DEF + 71);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 70: //����
	{
		pPlayer->CLOSE_GOSSIP_MENU();
		if (jf >= bagjf)
		{
		  ItemPosCountVec dest;
		  uint32 noSpaceForCount = 0;
		  pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 99004, 1, &noSpaceForCount);
		  Item* Pitem = pPlayer->StoreNewItem(dest, 99007, true, Item::GenerateItemRandomPropertyId(99004));
		  pPlayer->SendNewItem(Pitem, 1, true, false);
		  ChatHandler(pPlayer).PSendSysMessage("����ɹ���");
		  break;
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㣡��");
			break;
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1: //˲��
	{
		if (pPlayer->GetItemCount(99004) >= 1 ||
			pPlayer->GetItemCount(99005) >= 1 ||
			pPlayer->GetItemCount(99006) >= 1)
		{
			ChatHandler(pPlayer).PSendSysMessage("���Ѿ���˲����Ʒ�ˣ�");
		}
		else
		{
			pPlayer->ADD_GOSSIP_ITEM(10, "����˲���ײ�1", 1, GOSSIP_ACTION_INFO_DEF + 20);
			pPlayer->ADD_GOSSIP_ITEM(10, "����˲���ײ�2", 1, GOSSIP_ACTION_INFO_DEF + 21);
			pPlayer->ADD_GOSSIP_ITEM(10, "����˲���ײ�3", 1, GOSSIP_ACTION_INFO_DEF + 22);
			pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 20:
	{
		pPlayer->CLOSE_GOSSIP_MENU();
		uint32 item1jf;
		item1jf = 0;
		auto itemsfresult = pPlayer->PQuery(GameDB::WorldDB, "SELECT sfitem1jf FROM world_conf");
		if (itemsfresult)
		{
			auto field = itemsfresult->Fetch();
			item1jf = field[0].GetUInt32();
		}
		if (jf >= item1jf)
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u)", item1jf);
			ItemPosCountVec dest;
			uint32 noSpaceForCount = 0;
			pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 99004, 1, &noSpaceForCount);
			Item* Pitem = pPlayer->StoreNewItem(dest, 99004, true, Item::GenerateItemRandomPropertyId(99004));
			pPlayer->SendNewItem(Pitem, 1, true, false);
			ChatHandler(pPlayer).PSendSysMessage("����ɹ���");
			break;
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("�������㣡");
			break;
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 21:
	{
		pPlayer->CLOSE_GOSSIP_MENU();
		uint32 item2jf;
		item2jf = 0;
		auto itemsfresult = pPlayer->PQuery(GameDB::WorldDB, "SELECT sfitem2jf FROM world_conf");
		if (itemsfresult)
		{
			auto field = itemsfresult->Fetch();
			item2jf = field[0].GetUInt32();
		}
		if (jf >= item2jf)
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u)", item2jf);
			ItemPosCountVec dest;
			uint32 noSpaceForCount = 0;
			pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 99005, 1, &noSpaceForCount);
			Item* Pitem = pPlayer->StoreNewItem(dest, 99005, true, Item::GenerateItemRandomPropertyId(99005));
			pPlayer->SendNewItem(Pitem, 1, true, false);
			ChatHandler(pPlayer).PSendSysMessage("����ɹ���");
			break;
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("�������㣡");
			break;
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 22:
	{
		pPlayer->CLOSE_GOSSIP_MENU();
		uint32 item3jf;
		item3jf = 0;
		auto itemsfresult = pPlayer->PQuery(GameDB::WorldDB, "SELECT sfitem3jf FROM world_conf");
		if (itemsfresult)
		{
			auto field = itemsfresult->Fetch();
			item3jf = field[0].GetUInt32();
		}
		if (jf > item3jf)
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u)", item3jf);
			ItemPosCountVec dest;
			uint32 noSpaceForCount = 0;
			pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 99006, 1, &noSpaceForCount);
			Item* Pitem = pPlayer->StoreNewItem(dest, 99006, true, Item::GenerateItemRandomPropertyId(99006));
			pPlayer->SendNewItem(Pitem, 1, true, false);
			ChatHandler(pPlayer).PSendSysMessage("����ɹ���");
		break;
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("�������㣡");
			break;
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 2: //˫�츳
	{
		if (pPlayer->GetItemCount(99001) >= 1 ||
			pPlayer->GetItemCount(99002) >= 1 ||
			pPlayer->GetItemCount(99003) >= 1)
		{
			ChatHandler(pPlayer).PSendSysMessage("���Ѿ���˫�츳��Ʒ�ˣ�");
		}
		else
		{
			pPlayer->ADD_GOSSIP_ITEM(10, "����˫�츳�ײ�1", 1, GOSSIP_ACTION_INFO_DEF + 23);
			pPlayer->ADD_GOSSIP_ITEM(10, "����˫�츳�ײ�2", 1, GOSSIP_ACTION_INFO_DEF + 24);
			pPlayer->ADD_GOSSIP_ITEM(10, "����˫�츳�ײ�3", 1, GOSSIP_ACTION_INFO_DEF + 25);
			pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 23:
	{
		pPlayer->CLOSE_GOSSIP_MENU();
		uint32 item1jf;
		item1jf = 0;
		auto itemsfresult = pPlayer->PQuery(GameDB::WorldDB, "SELECT tfitem1jf FROM world_conf");
		if (itemsfresult)
		{
			auto field = itemsfresult->Fetch();
			item1jf = field[0].GetUInt32();
		}
		if (jf >= item1jf)
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u)", item1jf);
			ItemPosCountVec dest;
			uint32 noSpaceForCount = 0;
			pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 99001, 1, &noSpaceForCount);
			Item* Pitem = pPlayer->StoreNewItem(dest, 99001, true, Item::GenerateItemRandomPropertyId(99001));
			pPlayer->SendNewItem(Pitem, 1, true, false);
			ChatHandler(pPlayer).PSendSysMessage("����ɹ���");
			break;
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("�������㣡");
			break;
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 24:
	{
		pPlayer->CLOSE_GOSSIP_MENU();
		uint32 item2jf;
		item2jf = 0;
		auto itemsfresult = pPlayer->PQuery(GameDB::WorldDB, "SELECT tfitem2jf FROM world_conf");
		if (itemsfresult)
		{
			auto field = itemsfresult->Fetch();
			item2jf = field[0].GetUInt32();
		}
		if (jf >= item2jf)
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u)", item2jf);
			ItemPosCountVec dest;
			uint32 noSpaceForCount = 0;
			pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 99002, 1, &noSpaceForCount);
			Item* Pitem = pPlayer->StoreNewItem(dest, 99002, true, Item::GenerateItemRandomPropertyId(99002));
			pPlayer->SendNewItem(Pitem, 1, true, false);
			ChatHandler(pPlayer).PSendSysMessage("����ɹ���");
			break;
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("�������㣡");
			break;
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 25:
	{
		pPlayer->CLOSE_GOSSIP_MENU();
		uint32 item3jf;
		item3jf = 0;
		auto itemsfresult = pPlayer->PQuery(GameDB::WorldDB, "SELECT tfitem3jf FROM world_conf");
		if (itemsfresult)
		{
			auto field = itemsfresult->Fetch();
			item3jf = field[0].GetUInt32();
		}
		if (jf >= item3jf)
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u)", item3jf);
			ItemPosCountVec dest;
			uint32 noSpaceForCount = 0;
			pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 99003, 1, &noSpaceForCount);
			Item* Pitem = pPlayer->StoreNewItem(dest, 99003, true, Item::GenerateItemRandomPropertyId(99003));
			pPlayer->SendNewItem(Pitem, 1, true, false);
			break;
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("�������㣡");
			break;
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 3:
	{
		oldlevel = pPlayer->getLevel();
		uplevel = ((60 - oldlevel) * levelupjf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ������", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000014);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000015);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		ChatHandler(pPlayer).PSendSysMessage("�𾴵��������,�����ڵĵȼ�Ϊ%u��,������60����%u�����,��ȷ��!", oldlevel, uplevel);
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000014://��������
	{
		oldlevel = pPlayer->getLevel();
		uplevel = ((60 - oldlevel) * levelupjf);
		auto jf_xresult = pPlayer->PQuery(GameDB::WorldDB, "SELECT maxlevelupjf FROM world_conf");
		auto field = jfresult->Fetch();
		uint32 maxjf = field[0].GetUInt32();
		if (uplevel > maxjf)
		{
			uplevel = maxjf;
		}
		pPlayer->CLOSE_GOSSIP_MENU();
		if (jf >= uplevel)
		{
			if (uplevel)
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", uplevel, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("�𾴵����,��ϲ�����ɹ�!����%u�����������60����", uplevel);
			pPlayer->GiveLevel(60);
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ���!");
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000015:
	{
		pPlayer->CLOSE_GOSSIP_MENU();
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 6:
	{
		pPlayer->ADD_GOSSIP_ITEM(3, "�����ɿ�", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000101);
		pPlayer->ADD_GOSSIP_ITEM(3, "��������", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000102);
		pPlayer->ADD_GOSSIP_ITEM(3, "��������", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000103);
		pPlayer->ADD_GOSSIP_ITEM(3, "�����÷�", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000104);
		pPlayer->ADD_GOSSIP_ITEM(3, "�������", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000105);
		pPlayer->ADD_GOSSIP_ITEM(3, "������Ƥ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000106);
		pPlayer->ADD_GOSSIP_ITEM(3, "��������", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000107);
		pPlayer->ADD_GOSSIP_ITEM(3, "������ħ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000108);
		pPlayer->ADD_GOSSIP_ITEM(3, "�����鱦", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000109);
		pPlayer->ADD_GOSSIP_ITEM(3, "��������", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000110);
		pPlayer->ADD_GOSSIP_ITEM(3, "��������", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000111);
		pPlayer->ADD_GOSSIP_ITEM(3, "������ҩ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000112);
		pPlayer->ADD_GOSSIP_ITEM(3, "������Ƥ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000113);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000101:
	{
		if (jf >= 50 && pPlayer->HasSkill(186))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(186, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000102:
	{
		if (jf >= 50 && pPlayer->HasSkill(171))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(171, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000103:
	{
		if (jf >= 50 && pPlayer->HasSkill(164))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(164, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000104:
	{
		if (jf >= 50 && pPlayer->HasSkill(197))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(197, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000105:
	{
		if (jf >= 50 && pPlayer->HasSkill(185))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(185, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000106:
	{
		if (jf >= 50 && pPlayer->HasSkill(165))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(165, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000107:
	{
		if (jf >= 100 && pPlayer->HasSkill(356))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(356, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000108:
	{
		if (jf >= 50 && pPlayer->HasSkill(333))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(333, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000109:
	{
		if (jf >= 50 && pPlayer->HasSkill(755))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(755, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000110:
	{
		if (jf >= 50 && pPlayer->HasSkill(202))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(202, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000111:
	{
		if (jf >= 50 && pPlayer->HasSkill(129))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(129, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000112:
	{
		if (jf >= 50 && pPlayer->HasSkill(182))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(182, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 1000113:
	{
		if (jf >= 50 && pPlayer->HasSkill(393))
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			pPlayer->SetSkill(393, 300, 300);
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", maxskilljf, pPlayer->GetSession()->GetAccountId());
			ChatHandler(pPlayer).PSendSysMessage("��л����֧�֣�");
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("���ֲ��㹺�����Ŀ,��δѧ�øü��ܣ�");
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	case GOSSIP_ACTION_INFO_DEF + 5://��ҵ���ܹ���
		if (skillcount < maxskillcount)
		{
			if (!pPlayer->HasSpell(2575))
			{
				pPlayer->ADD_GOSSIP_ITEM(0, "ѧϰ�ɿ�", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 51);
			}
			if (!pPlayer->HasSpell(2259))
			{
				pPlayer->ADD_GOSSIP_ITEM(0, "ѧϰ����", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 52);
			}
			if (!pPlayer->HasSpell(2018))
			{
				pPlayer->ADD_GOSSIP_ITEM(0, "ѧϰ����", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 53);
			}
			if (!pPlayer->HasSpell(3908))
			{
				pPlayer->ADD_GOSSIP_ITEM(0, "ѧϰ�÷�", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 54);
			}
			if (!pPlayer->HasSpell(2108))
			{
				pPlayer->ADD_GOSSIP_ITEM(0, "ѧϰ��Ƥ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 55);
			}
			if (!pPlayer->HasSpell(7414))
			{
				pPlayer->ADD_GOSSIP_ITEM(0, "ѧϰ��ħ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 56);
			}
			if (!pPlayer->HasSpell(4036))
			{
				pPlayer->ADD_GOSSIP_ITEM(0, "ѧϰ����", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 58);
			}
			if (!pPlayer->HasSpell(2372))
			{
				pPlayer->ADD_GOSSIP_ITEM(0, "ѧϰ��ҩ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 59);
			}
			if (!pPlayer->HasSpell(8613))
			{
				pPlayer->ADD_GOSSIP_ITEM(0, "ѧϰ��Ƥ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 60);
			}
			pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
			return true;
		}
		else
		{
			pPlayer->GetSession()->SendNotification("רҵ���ܹ��࣬�޷�����ѧϰ��");
			return true;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	case GOSSIP_ACTION_INFO_DEF + 51://����ɿ�
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ��ѧϰ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5001);
		ChatHandler(pPlayer).PSendSysMessage("�𾴵����|cff54FF9F[%s]|r����,���β������۳�|cffFF0000%u|r�����,��ȷ��!", pPlayer->GetName(), learnskilljf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5002);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;

	case GOSSIP_ACTION_INFO_DEF + 52://��������
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ��ѧϰ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5003);
		ChatHandler(pPlayer).PSendSysMessage("�𾴵����|cff54FF9F[%s]|r����,���β������۳�|cffFF0000%u|r�����,��ȷ��!", pPlayer->GetName(), learnskilljf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5004);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;

	case GOSSIP_ACTION_INFO_DEF + 53://�������
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ��ѧϰ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5005);
		ChatHandler(pPlayer).PSendSysMessage("�𾴵����|cff54FF9F[%s]|r����,���β������۳�|cffFF0000%u|r�����,��ȷ��!", pPlayer->GetName(), learnskilljf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5006);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;

	case GOSSIP_ACTION_INFO_DEF + 54://����÷�
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ��ѧϰ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5007);
		ChatHandler(pPlayer).PSendSysMessage("�𾴵����|cff54FF9F[%s]|r����,���β������۳�|cffFF0000%u|r�����,��ȷ��!", pPlayer->GetName(), learnskilljf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5008);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;

	case GOSSIP_ACTION_INFO_DEF + 55://������Ƥ
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ��ѧϰ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5009);
		ChatHandler(pPlayer).PSendSysMessage("�𾴵����|cff54FF9F[%s]|r����,���β������۳�|cffFF0000%u|r�����,��ȷ��!", pPlayer->GetName(), learnskilljf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5010);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;

	case GOSSIP_ACTION_INFO_DEF + 56://����ħ
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ��ѧϰ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5011);
		ChatHandler(pPlayer).PSendSysMessage("�𾴵����|cff54FF9F[%s]|r����,���β������۳�|cffFF0000%u|r�����,��ȷ��!", pPlayer->GetName(), learnskilljf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5012);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;

	case GOSSIP_ACTION_INFO_DEF + 57://�����鱦
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ��ѧϰ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5013);
		ChatHandler(pPlayer).PSendSysMessage("�𾴵����|cff54FF9F[%s]|r����,���β������۳�|cffFF0000%u|r�����,��ȷ��!", pPlayer->GetName(), learnskilljf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5014);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;

	case GOSSIP_ACTION_INFO_DEF + 58://���򹤳�
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ��ѧϰ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5015);
		ChatHandler(pPlayer).PSendSysMessage("�𾴵����|cff54FF9F[%s]|r����,���β������۳�|cffFF0000%u|r�����,��ȷ��!", pPlayer->GetName(), learnskilljf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5016);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;

	case GOSSIP_ACTION_INFO_DEF + 59://�����ҩ
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ��ѧϰ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5017);
		ChatHandler(pPlayer).PSendSysMessage("�𾴵����|cff54FF9F[%s]|r����,���β������۳�|cffFF0000%u|r�����,��ȷ��!", pPlayer->GetName(), learnskilljf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5018);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;

	case GOSSIP_ACTION_INFO_DEF + 60://�����Ƥ
		pPlayer->ADD_GOSSIP_ITEM(0, "ȷ��ѧϰ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5019);
		ChatHandler(pPlayer).PSendSysMessage("�𾴵����|cff54FF9F[%s]|r����,���β������۳�|cffFF0000%u|r�����,��ȷ��!", pPlayer->GetName(), learnskilljf);
		pPlayer->ADD_GOSSIP_ITEM(0, "ȡ��", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5020);
		pPlayer->SEND_GOSSIP_MENU(822, pItem->GetGUID());
		return true;
				/////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////
		//����ɿ���//
		////////////////
	case GOSSIP_ACTION_INFO_DEF + 5001:
		if (jf <= learnskilljf)
		{
			pPlayer->Say("���ֲ���!", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		else
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", learnskilljf, pPlayer->GetSession()->GetAccountId());
			pPlayer->learnSpell(2575,false);
			pPlayer->Say("����ɹ���", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 5002:
	{
		 pPlayer->Say("��лʹ��", LANG_UNIVERSAL);
		 pPlayer->CLOSE_GOSSIP_MENU();
	}
		break;

		////////////////
		//����������//
		////////////////
	case GOSSIP_ACTION_INFO_DEF + 5003:
		if (jf <= learnskilljf)
		{
			pPlayer->Say("���ֲ���!", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		else if (pPlayer->HasSpell(2259))
		{
			pPlayer->Say("�Ѿ�ӵ��������!", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		else
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", learnskilljf, pPlayer->GetSession()->GetAccountId());
			pPlayer->learnSpell(2259, false);
			pPlayer->Say("����ɹ���", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 5004:
	{
		pPlayer->Say("��лʹ��", LANG_UNIVERSAL);
		pPlayer->CLOSE_GOSSIP_MENU();
	}
		break;
		////////////////
		//������켼��//
		////////////////
	case GOSSIP_ACTION_INFO_DEF + 5005:
		if (jf <= learnskilljf)
		{
			pPlayer->Say("���ֲ���!", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		else
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", learnskilljf, pPlayer->GetSession()->GetAccountId());
			pPlayer->learnSpell(2018, false);
			pPlayer->Say("����ɹ���", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 5006:
	{
		pPlayer->Say("��лʹ��", LANG_UNIVERSAL);
		pPlayer->CLOSE_GOSSIP_MENU();
	}
		break;
		////////////////
		//����÷켼��//
		////////////////
	case GOSSIP_ACTION_INFO_DEF + 5007:
		if (jf <= learnskilljf)
		{
			pPlayer->Say("���ֲ���!", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		else
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", learnskilljf, pPlayer->GetSession()->GetAccountId());
			pPlayer->learnSpell(3908, false);
			pPlayer->Say("����ɹ���", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 5008:
	{
		pPlayer->Say("��лʹ��", LANG_UNIVERSAL);
		pPlayer->CLOSE_GOSSIP_MENU();
	}
		break;

		////////////////
		//������Ƥ����//
		////////////////
	case GOSSIP_ACTION_INFO_DEF + 5009:
		if (jf <= learnskilljf)
		{
			pPlayer->Say("���ֲ���!", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		else
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", learnskilljf, pPlayer->GetSession()->GetAccountId());
			pPlayer->learnSpell(2108, false);
			pPlayer->Say("����ɹ���", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 5010:
	{
		    pPlayer->Say("��лʹ��", LANG_UNIVERSAL);
		    pPlayer->CLOSE_GOSSIP_MENU();
	}
		break;
		////////////////
		//����ħ����//
		////////////////
	case GOSSIP_ACTION_INFO_DEF + 5011:
		if (jf <= learnskilljf)
		{
			pPlayer->Say("���ֲ���!", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		else
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", learnskilljf, pPlayer->GetSession()->GetAccountId());
			pPlayer->learnSpell(7414, false);
			pPlayer->Say("����ɹ���", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 5012:
	{
		pPlayer->Say("��лʹ��", LANG_UNIVERSAL);
		pPlayer->CLOSE_GOSSIP_MENU();
	}
		break;
		////////////////
		//���򹤳̼���//
		////////////////
	case GOSSIP_ACTION_INFO_DEF + 5015:
		if (jf <= learnskilljf)
		{
			pPlayer->Say("���ֲ���!", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		else
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", learnskilljf, pPlayer->GetSession()->GetAccountId());
			pPlayer->learnSpell(4036, false);
			pPlayer->Say("����ɹ���", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 5016:
	{
		pPlayer->Say("��лʹ��", LANG_UNIVERSAL);
		pPlayer->CLOSE_GOSSIP_MENU();
	}
		break;
		////////////////
		//�����ҩ����//
		////////////////
	case GOSSIP_ACTION_INFO_DEF + 5017:
		if (jf <= learnskilljf)
		{
			pPlayer->Say("���ֲ���!", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		else
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", learnskilljf, pPlayer->GetSession()->GetAccountId());
			pPlayer->learnSpell(2372, false);
			pPlayer->Say("����ɹ���", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 5018:
	{
		pPlayer->Say("��лʹ��", LANG_UNIVERSAL);
		pPlayer->CLOSE_GOSSIP_MENU();
	}
		break;
		////////////////
		//�����Ƥ����//
		////////////////
	case GOSSIP_ACTION_INFO_DEF + 5019:
		if (jf <= learnskilljf)
		{
			pPlayer->Say("���ֲ���!", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		else
		{
			pPlayer->PExecute(GameDB::RealmDB, "UPDATE account SET jf = (jf - %u) WHERE id = %u", learnskilljf, pPlayer->GetSession()->GetAccountId());
			pPlayer->learnSpell(8613, false);
			pPlayer->Say("����ɹ���", LANG_UNIVERSAL);
			pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 5020:
	{
		pPlayer->Say("��лʹ��", LANG_UNIVERSAL);
		pPlayer->CLOSE_GOSSIP_MENU();
	}
		break;
	}
	return true;
}




void AddSC_transform()
{
	Script* pNewScript;
	pNewScript = new Script;
	pNewScript->Name = "frozen_transform";
	pNewScript->pItemUse = &ItemUse_frozen_transform;
	pNewScript->pGossipSelectItem = &ItemSelect_frozen_transform;
	pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "frozen_detransform";
	pNewScript->pItemUse = &ItemUse_frozen_detransform;
	pNewScript->pItemSelect = &ItemSelect_frozen_detransform;
	pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "item_teleport";
	pNewScript->pItemUse = &ItemUse_Item_TelePort;
	pNewScript->pGossipSelectItem = &ItemSelect_Item_TelePort;
	pNewScript->RegisterSelf();
}