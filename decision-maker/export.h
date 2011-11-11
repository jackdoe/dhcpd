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
 ECOPY(global.my_mac,reply_packet->p.eh.ether_shost);
 reply_packet->p.eh.ether_type = htons(ETHERTYPE_IP);

 if (request_packet->d.flags != 0)
	ECOPY(bc_mac,reply_packet->p.eh.ether_dhost);
 else 
	ECOPY(request_packet->p.eh.ether_shost,reply_packet->p.eh.ether_dhost); 
 
 
 struct ip *ip=(struct ip *) &reply_packet->p.ip;
 ip->ip_v = 4;
 ip->ip_hl = 20/4;
 ip->ip_tos = IPTOS_LOWDELAY;
 ip->ip_len = htons(sizeof(*reply_packet) - sizeof(struct ether_header));
 ip->ip_id = 0;
 ip->ip_off = 0;
 ip->ip_ttl = 0xF;
 ip->ip_p = IPPROTO_UDP;
 ip->ip_sum = 0;
 ip->ip_src = global.siaddr;
 struct udphdr *udp=(struct udphdr *) &reply_packet->p.udp;
 udp->uh_sport=htons(67);
 udp->uh_dport=htons(68);
 udp->uh_ulen=htons(sizeof(*reply_packet) - sizeof(struct ether_header) - sizeof(struct ip));
 udp->uh_sum=0;
 
 reply_packet->d.op = BOOTREPLY;
 reply_packet->d.htype = 1; 
 reply_packet->d.hlen = ETHER_ADDR_LEN;
 reply_packet->d.hops = 0;
 reply_packet->d.xid = request_packet->d.xid;
 reply_packet->d.secs = 0;
 reply_packet->d.flags = request_packet->d.flags;
 reply_packet->d.ciaddr.s_addr = 0;
 reply_packet->d.yiaddr = l->ip;
 reply_packet->d.giaddr = request_packet->d.giaddr;
 bcopy(request_packet->d.chaddr,reply_packet->d.chaddr,16);
 
 reply_packet->d.cookie = htonl(DHCP_COOKIE);
 
 
 if (request_packet->d.flags != 0) 
	reply_packet->p.ip.ip_dst.s_addr = INADDR_BROADCAST; 
 else 
	reply_packet->p.ip.ip_dst = l->ip;
 reply_packet->o.msg.code = 53;
 reply_packet->o.msg.len = 1;
 reply_packet->o.msg.msg_type = reply_msg_type;
 
 reply_packet->o.server_id.code = 54;
 reply_packet->o.server_id.len = 4;
 reply_packet->o.server_id.addr = global.siaddr;
 reply_packet->o.end.code = 255;
 
 
 reply_packet->o.lease_time.code = 51;
 reply_packet->o.lease_time.len = 4;
 reply_packet->o.lease_time.lease_time = htonl(l->lease_time);
 reply_packet->o.subnet.code = 1;
 reply_packet->o.subnet.len = 4;
 reply_packet->o.subnet.addr = l->mask;
 reply_packet->o.broadcast.code = 28;
 reply_packet->o.broadcast.len = 4;
 reply_packet->o.broadcast.addr = l->br;
 
 reply_packet->o.gateway.code = 3;
 reply_packet->o.gateway.len = 4;
 reply_packet->o.gateway.addr = l->gw;

 reply_packet->o.dns.code = 6;
 reply_packet->o.dns.len = sizeof(struct in_addr) * DNS_COUNT;
 int i;
 for (i=0;i<DNS_COUNT;i++) {
	reply_packet->o.dns.addr[i] = l->dns[i];
 }
  */
