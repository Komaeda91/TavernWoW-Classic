ALTER TABLE db_version CHANGE COLUMN required_z0759_s0065_01_mangos_creature_addon required_z0763_s0069_01_mangos_reputation_reward_rate bit;

DROP TABLE IF EXISTS `reputation_reward_rate`;
CREATE TABLE `reputation_reward_rate` (
  `faction` mediumint(8) unsigned NOT NULL default '0',
  `quest_rate` float NOT NULL default '1',
  `creature_rate` float NOT NULL default '1',
  `spell_rate` float NOT NULL default '1',
  PRIMARY KEY  (`faction`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
