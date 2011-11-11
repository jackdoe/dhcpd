#include "../../dhcpd.h"
#include "../export.h"
#include <my_global.h>
#include <mysql.h>
#include "mysql-config.h"

extern struct __global global;
struct lease {
	struct in_addr ip;
	struct in_addr subnet;
	struct in_addr dns[DNS_COUNT];
	struct in_addr gw;
	struct in_addr broadcast;
	u32 lease_time;
} __packed;
#define QUERY(arg...) snprintf(query,sizeof(query),##arg)
MYSQL *conn;
static void fill(struct dhcp_reply *reply_packet,
				 struct dhcp_packet *request_packet, 
				 struct lease *l,u8 reply_msg_type);

static void sql(char *query);
int p_reply(struct dhcp_reply *reply_packet,
			struct dhcp_packet *request_packet,
			u8 reply_msg_type, u8 request_msg_type) {	
	char query[256];
	MYSQL_RES *result;
	MYSQL_ROW row;
	struct lease l;
	u8 ret = FAIL;
	QUERY("SELECT a.ip,b.subnet,b.dns1,b.dns2,b.gw,b.broadcast,b.leasetime FROM `framed` a,`range` b WHERE a.mac = '" P_ETH "' AND a.range_id = b.id",
		  P_ETHARG(request_packet->d.chaddr));

	sql(query);
	result = mysql_store_result(conn);
	if (result) {
		row = mysql_fetch_row(result);
		if (row) {
			//a.ip,b.subnet,b.dns1,b.dns2,b.gw,b.broadcast,b.leasetime
			inet_aton(row[0],&l.ip);
			inet_aton(row[1],&l.subnet);
			inet_aton(row[2],&l.dns[0]);
			inet_aton(row[3],&l.dns[1]);
			inet_aton(row[4],&l.gw);
			inet_aton(row[5],&l.broadcast);
			l.lease_time = atoi(row[6]);
			fill(reply_packet,request_packet,&l,reply_msg_type);
			ret = SUCCESS;
		}
		mysql_free_result(result);
	}
	return ret;
}

int p_release(u8 *mac,struct in_addr ip,struct dhcp_packet *packet) {
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
	conn = mysql_init(NULL);
	
	if (conn == NULL) {
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		clean_exit(EXIT_FAILURE);
	}
	if (mysql_real_connect(conn, DB_HOST, DB_USER, 
						   DB_PASS, DB_DATABASE, DB_PORT, NULL, 0) == NULL) {
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		clean_exit(EXIT_FAILURE);
	}
	*init_status = SUCCESS;
}
void p_cleanup(void) {
	if (conn) {
		mysql_close(conn);
	}
}
static void fill(struct dhcp_reply *reply_packet,
				 struct dhcp_packet *request_packet, 
				 struct lease *l,u8 reply_msg_type) {
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

static void sql(char *query) {
	_E(V_NOTICE,"query: %s",query);
	mysql_query(conn, query);
	return;
}
