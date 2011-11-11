#include <stdio.h>
#include <arpa/inet.h>
#include "../../dhcpd.h"
#include "static.h"

#include <sys/types.h>
#include <db.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h> /* S_IRWXU */

/* 
 * example insert program 
 * cc -o modify modify.c
 * usage: ./modify 00:11:22:33:44:55 1.2.3.4
 * sets key->value pair with key 00:11:22:33:44:55
 *
 * without any arguments it dumps the contents
 * of the db file
 */
#define SUBNET "255.255.255.0"
#define GATEWAY "10.100.100.1"
#define BROADCAST "10.100.100.255"
#define DNS1 "8.8.8.8"
#define DNS2 "8.8.4.4"
#define LEASE_TIME 3600
DB *db;
static void modify(struct in_addr ip, u8 *mac);
static void dump(void);
static int _ether_aton(char *s,u_char *dest);
int main(int ac, char **av) {
	struct in_addr ip;
	u8 mac[ETHER_ADDR_LEN];

	db = dbopen(DB_FILE, O_CREAT | O_RDWR, S_IRWXU, DB_HASH, NULL);
	if (!db) {
		perror("dbopen");
		errx(EXIT_FAILURE,"cant open %s",DB_FILE);
	}
	if (ac == 1) {
		dump();
		goto ret;
	} 

	if (ac < 3) 
		errx(EXIT_FAILURE,"need 2 args, %s mac ip (key value)",av[0]);
	if (!inet_aton(av[2],&ip))
		errx(EXIT_FAILURE,"bad ip: %s",av[2]);
	if (!_ether_aton(av[1],mac)) 
		errx(EXIT_FAILURE,"bad mac: %s",av[1]);
	
	modify(ip,mac);
ret:
	db->close(db);
	return(0);
}
static void modify(struct in_addr ip, u8 *mac) {
	struct lease l;
	bzero(&l,sizeof(l));
	l.ip = ip;
	inet_aton(SUBNET,&l.subnet);
	inet_aton(GATEWAY,&l.gw);
	inet_aton(BROADCAST,&l.broadcast);
	inet_aton(DNS1,&l.dns[0]);
	inet_aton(DNS2,&l.dns[1]);
	DBT key,value;
	key.size = ETHER_ADDR_LEN;
	key.data = mac;
	value.size = sizeof(l);
	value.data = &l;
	db->del(db,&key,0);
	db->put(db,&key,&value,R_NOOVERWRITE);
	db->sync(db,0);
}
static int _ether_aton(char *s,u_char *dest) {
	register int i;
	unsigned int o0, o1, o2, o3, o4, o5;
	i = sscanf(s, "%x:%x:%x:%x:%x:%x", &o0, &o1, &o2, &o3, &o4, &o5);
	if (i != 6)
		return (0);
	dest[0]= (u_char) o0;
	dest[1]= (u_char) o1;
	dest[2]= (u_char) o2;
	dest[3]= (u_char) o3;
	dest[4]= (u_char) o4;
	dest[5]= (u_char) o5;
	return (1);
}
static void dump(void) {
	DBT key,value;
	struct lease *l;
	u8 *mac;
	while (db->seq(db,&key,&value,R_NEXT) == 0) {
		l = (struct lease *) value.data;
		mac = (u8 *) key.data;
		printf(P_ETH " -> %s\n",P_ETHARG(mac),inet_ntoa(l->ip));
	}
}
