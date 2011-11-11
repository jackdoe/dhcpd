/*
 * p_reply()
 * arguments:
 *	reply_packet - 
 *		poiter to a dhcp_reply structure, the decision maker must fill
 *		the whole structure, including ether_header, ip header,udp header
 *		dhcp fixed header, and dhcp options
 *	request_packet - is just a pointer to the DHCPDISCOVER/OFFER packet
 *
 * returns:
 *	SUCCESS	- if there is a lease available (server will reply)
 *	FAIL	- no free leases (server will do nothing)
 * 
 */
int p_reply(struct dhcp_reply *reply_packet,
			struct dhcp_packet *request_packet, 
			u8 reply_msg_type, u8 request_msg_type);

/*
 * p_release()
 * returns:
 *	SUCCESS	- if there was a lease (the server will execute expiry() event)
 *	FAIL	- if there was no such lease (server will do nothing)
 */
int p_release(u8 *mac,struct in_addr ip,struct dhcp_packet *request_packet);
void p_log(struct dhcp_packet *request_packet, int message);

/*
 * do whatever you want in a regular time intervals (server uses alarm())
 * and it has safe signal handler called from pcap_callback, every pcap timeout
 * currently signals that can handled by the plugin: SIGALRM, SIGUSR1,SIGUSR1
 */
#ifndef SIGNAL_INTERVAL
#define SIGNAL_INTERVAL 60
#endif
void p_signal(u8 sig);
void p_init(int *init_status);
void p_cleanup(void);

/*
example of p_reply function:
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
*/
