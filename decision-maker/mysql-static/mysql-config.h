#ifndef _mysql_config_h
#define _mysql_config_h
#define DB_HOST "localhost"
#define DB_USER "dhcp"
#define DB_PASS "*SA&DCXHZ^@SAJDJ"
#define DB_PORT 3306
#define DB_DATABASE "dhcp"
/*
use `dhcp`
DROP TABLE IF EXISTS `framed`;
CREATE TABLE `framed` (
					   `id` bigint(20) NOT NULL DEFAULT '0',
					   `ip` char(16) NOT NULL,
					   `mac` char(17) NOT NULL,
					   `range_id` bigint(20) NOT NULL DEFAULT '0',
					   PRIMARY KEY (`id`),
					   UNIQUE KEY `ip` (`ip`),
					   UNIQUE KEY `mac` (`mac`)
					   );
DROP TABLE IF EXISTS `range`;
CREATE TABLE `range` (
					  `id` bigint(20) NOT NULL DEFAULT '0',
					  `gw` char(16) NOT NULL,
					  `subnet` char(16) NOT NULL,
					  `broadcast` char(16) NOT NULL,
					  `dns1` char(16) NOT NULL,
					  `dns2` char(16) NOT NULL,
					  PRIMARY KEY (`id`)
					  );

*/
#endif
