#include "../../dhcpd.h"
#include "../export.h"
#include "static.h"
#include <sys/types.h>
#include <db.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h> /* S_IRWXU */

/* 
 * example for reading a file and using it as reference for static address
 * assignment
 */

DB *db;
extern struct __global global;
static void fill(struct dhcp_reply *reply_packet,struct dhcp_packet *request_packet, struct lease *l,u8 reply_msg_type);
int p_reply(struct dhcp_reply *reply_packet,
			struct dhcp_packet *request_packet,
			u8 reply_msg_type, u8 request_msg_type) {
	
	DBT key,value;
	key.size = ETHER_ADDR_LEN;
	key.data = request_packet->d.chaddr;
	if (db->get(db,&key,&value,0) == 0) {
		fill(reply_packet,request_packet,(struct lease *) value.data,reply_msg_type);
		return SUCCESS;
	} else {
		_E(V_WARN,P_ETH " not found in %s",P_ETHARG(request_packet->d.chaddr),
		   DB_FILE);
	}
	return FAIL;
}

int p_release(u8 *mac,struct in_addr ip,struct dhcp_packet *packet) {
	_E(V_ERR,"cant release lease from dummy plugin");
	return FAIL;
}
void p_event(uint8_t *mac, struct in_addr ip, int message,int ttl) {
}

void p_signal(u8 sig) {
	_E(V_ERR,"aa..nothing to do signal: %u",sig);
}

void p_log(struct dhcp_packet *request_packet, int message) {
	return;
}
void p_init(int *init_status) {
	_E(V_NOTICE,"opening %s",DB_FILE);

	db = dbopen(DB_FILE, O_CREAT | O_RDWR, S_IRWXU, DB_HASH, NULL);
	if (!db) {
		perror("dbopen");
		_E(V_CRY,"cant open %s",DB_FILE);
		clean_exit(EXIT_FAILURE);
	}
	*init_status = SUCCESS;
}
void p_cleanup(void) {
	if (db)
		db->close(db);
}
static void fill(struct dhcp_reply *reply_packet,struct dhcp_packet *request_packet, struct lease *l,u8 reply_msg_type) {
	struct options *o = &reply_packet->o;
	o_add_u8(o,O_MSGTYPE,reply_msg_type);
	o_add_u32(o,O_SRV_ID,global.siaddr.s_addr);
	o_add_u32(o,O_LEASE_TIME,htonl(l->lease_time));
	o_add_u32(o,O_SUBNET,l->subnet.s_addr);
	o_add_u32(o,O_BROADCAST,l->broadcast.s_addr);
	o_add_u32(o,O_ROUTER,l->gw.s_addr);
	o_add(o,O_DNS,4*(sizeof(l->dns)/sizeof(l->dns[0])),&l->dns[0]);
	o_end(o);
	fill_eth_ip_udp_dh_hdr(request_packet,reply_packet,l->ip);	
}

