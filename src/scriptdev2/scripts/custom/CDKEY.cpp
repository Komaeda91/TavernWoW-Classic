#include "precompiled.h"
#include "ObjectMgr.h"

#if _MSC_VER >= 1600 // VC2010
#pragma execution_character_set("utf-8")
#endif

bool GossipHello_custom_cdkey(Player* pPlayer, Creature* pCreature)
{
	pPlayer->ADD_GOSSIP_ITEM_EXTENDED(0, "���ƹ�С����������һ���", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "|cffEEB422�����ƹ�С�����仳�ɷ��������ܡ���|r \n\n QQȺ: 185738294|r\n\n |cffCD6889�ٷ���վ:http://www.wswow.online/ |r \n\n |cffAB82FF���ֹ�ƽ������ɫ�������������\n\n �ܾ�GM������Ϸ\n\n �ܾ�����װ��������\n\n �ܾ��κ�˽�¹����������ؼ��� \n\n �ܾ��κ�Ӱ����Ϸ��������Ϊ\n\n ����ά�����õ���Ϸ��Χ|r \n\n |cffCDBE70������ħ����ɫϵͳ���ܡ��|r  \n\n |cffCAFF70��û�ϵͳ��|r,|cffCAE1FF�����ϵͳ��|r\n\n |cffBF3EFF��˫���츳��|r,|cff8B7500��ս����ļ��|r \n\n |cffFF3E96��ǰ׺��ħ��|r,|cffFFBBFF��˲��ϵͳ��|r \n\n |cffFF7F24�����ɫ��ͼ���|r,|cffFFD700��������ܡ��|r \n\n |cffCAE1FF�����ֹ��ܳ���������...���|r", 0, true);
	pPlayer->SEND_GOSSIP_MENU(822, pCreature->GetGUID());
	return true;
}

bool GossipSelectWithCode_custom_cdkey(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action, const char* sCode)
{
	if (action == GOSSIP_ACTION_INFO_DEF + 1)
	{
		auto cdkey_result = pPlayer->PQuery(GameDB::WorldDB, "SELECT id FROM cdkey WHERE cdkey ='%s'", sCode);//�Զ���Ѱƥ��scode��cdkey�ֶ�
		if (cdkey_result) //���cdkey��scodeƥ��
		{
			auto field = cdkey_result->Fetch(); //�Զ���ƥ������е����ݱ�������?
			uint32 id = field[0].GetUInt32(); //��(��������?)����0��䵽(id)
			auto items_result = pPlayer->PQuery(GameDB::WorldDB,"SELECT item,count FROM cdkey_items WHERE cdkey_id = %u", id);
			do
			{
				auto item_field = items_result->Fetch();
				uint32 item = item_field[0].GetUInt32();
				uint32 count = item_field[1].GetUInt32();
				//ItemPrototype const* markProto = sObjectMgr->::GetItemPrototype(item);
				ItemPosCountVec dest;
				uint32 noSpaceForCount = 0;
				pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item, count, &noSpaceForCount);
				Item* Pitem = pPlayer->StoreNewItem(dest, item, true, Item::GenerateItemRandomPropertyId(item));
				pPlayer->SendNewItem(Pitem, count, true, false);
				pPlayer->GetSession()->SendNotification("��ȡ�ɹ���");
			} while (items_result->NextRow());
			pPlayer->PExecute(GameDB::WorldDB,"DELETE FROM cdkey WHERE id=%u", id);
			pPlayer->PExecute(GameDB::WorldDB,"DELETE FROM cdkey_items WHERE cdkey_id=%u", id);
		}
		else
		{
			ChatHandler(pPlayer).PSendSysMessage("CDKEY����,�޷���ȡ");
			return true;
		}
	}
	return true;
}

void AddSC_custom_cdkey()
{
	Script* script;
	script = new Script();
	script->Name = "custom_cdkey";
	script->pGossipHello = &GossipHello_custom_cdkey;
	script->pGossipSelectWithCode = &GossipSelectWithCode_custom_cdkey;
	script->RegisterSelf();
}