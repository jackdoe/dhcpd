#include "dhcpd.h"
#include "decision-maker/export.h"
#include "ip-util.h"

struct __global global;
u8 zero_mac[ETHER_ADDR_LEN] = {0,0,0,0,0,0};
u8 bc_mac[ETHER_ADDR_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff};
pcap_t *cap;
char errbuf[PCAP_ERRBUF_SIZE];

void sig_handler(int sig);
void clean_exit(int rc);

static void usage(void);
static void signal_init(void);
static int file_exist(char *file);
static void pcap_start(void);
static void pid_file(char *file, int action);
static void socket_bind(void);
void safe_signal_handle(int sig);
static void pcap_callback(u_char *user, 
			  const struct pcap_pkthdr *h,
			  const u_char *sp);
static void process(struct dhcp_packet *packet,ssize_t rlen);
static int setmac(u8 *dest,char *ifname);
static int validate(struct dhcp_packet *packet, int rlen);
static int gen_reply(struct dhcp_packet *input_packet, 
		     struct dhcp_reply *reply_packet,
		     u8 request_msg_type, 
		     u8 reply_msg_type);
static u16 reply_packet_len(struct dhcp_reply *r);

static void discover(struct dhcp_packet *p,ssize_t rlen);
static void request(struct dhcp_packet *p,ssize_t rlen);
static void release(struct dhcp_packet *p,ssize_t rlen);
static void inform(struct dhcp_packet *p,ssize_t rlen);
static void decline(struct dhcp_packet *p,ssize_t rlen);
char *msg2string[] = { 
	"UNDEFINED",
	"DHCPDISCOVER",
	"DHCPOFFER",
	"DHCPREQUEST",
	"DHCPDECLINE",
	"DHCPACK",
	"DHCPNACK",
	"DHCPRELEASE",
	"DHCPINFORM",
	"DHCPFORCERENEW",
	"DHCPLEASEQUERY",
	"DHCPLEASEUNASSIGNED",
	"DHCPLEASEUNKNOWN",
	"DHCPLEASEACTIVE"
};
int main(int argc, char *argv[]){
	int ch;
	u8 required = 2;
	bzero(&global,sizeof(global));
	gethostname(global.id,sizeof(global.id)-1); /* 
												 * XXX should let 
												 * plugin to decide 
												 */
	global.pidfile = strdup(PID_FILE);
	while ((ch = getopt(argc, argv, "fi:s:lvhp:")) != -1) {
		switch(ch) {
		case 'v':
			global.verbose++;
			break;
		case 'f':
			global.stderr_log = 1;
			break;
		case 'i':
			global.ifname = strndup(optarg,MAX_IFNAME_LEN);
			if (global.ifname && setmac(global.my_mac,global.ifname) == SUCCESS)
				required--;
			else
				SAYX("bad interface: %s",optarg);
			break;	
		case 's':
			if (!inet_aton(optarg,&global.siaddr))
				SAYX("bad ip: %s",optarg);
			
			required--;
			break;
		case 'p':
			global.pidfile = strndup(optarg,MAX_PIDFILE_LEN);
			break;
		case 'h':
			usage();
			return 0;
			break;
		}
	}
	if (global.ifname == NULL || required != 0) {
		usage();
		SAYX("wrong arguments need %d more required arguments",required);
	}
	_E(V_INFO,"server identifier: %s",global.id);
	_E(V_INFO,"starting dhcp server on interface: %s with source mac " \
	   P_ETH " and siaddr: %s",
	   global.ifname,P_ETHARG(global.my_mac),
	   inet_ntoa(global.siaddr));
	

	if (global.stderr_log == 0) 
		daemon(0,1);
	pid_file(global.pidfile,PID_CREATE);
	signal_init(); 
	socket_bind();
	p_init(&global.p_init_status);
	pcap_start(); /* loop waiting */
	clean_exit(EXIT_SUCCESS);
	return 0;
}
void pcap_callback(u_char *user, const struct pcap_pkthdr *h, const u_char *sp) {
	if (!sp || h->len == 0)
			return;
	struct dhcp_packet *packet = (struct dhcp_packet *) sp;
	if (validate(packet,h->len) == FAIL) 
			return;
	process(packet,h->len);
}

static int gen_reply(struct dhcp_packet *request_packet, 
					 struct dhcp_reply *reply_packet,
					 u8 request_msg_type, u8 reply_msg_type) {

	bzero(reply_packet,sizeof(*reply_packet));
	if (p_reply(reply_packet,
				request_packet,
				reply_msg_type,request_msg_type) == FAIL)
			return FAIL;
	
	ip_checksum(&reply_packet->p.ip,reply_packet_len(reply_packet) - EHDR);
	return SUCCESS;
}


static void process(struct dhcp_packet *packet,ssize_t rlen) {
	/* http://tools.ietf.org/html/rfc2132  */
	
	u8 type = o_find_u8(O_MSGTYPE,packet->options,rlen - DHLEN);
	if (type == 0 || type > sizeof(msg2string) - 1) {
		_E(V_WARN,"bad option 53 message type: %u", type)
		return;
	}
	
	p_log(packet,type);
	_E(V_NOTICE,"%s from " P_ETH " via %s",
	   msg2string[type],
	   P_ETHARG(packet->d.chaddr),global.ifname);

	switch (type) {
	case DHCPDISCOVER:
		discover(packet,rlen);
		break;
	case DHCPREQUEST:
		request(packet,rlen);
		break;
	case DHCPRELEASE:
		release(packet,rlen);
		break;
	case DHCPINFORM:
		inform(packet,rlen);
		break;
	case DHCPDECLINE:
		decline(packet,rlen);
		break;
	case DHCPLEASEQUERY:
		/* p_leasequery() */
		break;
	case DHCPOFFER:                                                                
	case DHCPACK:
	case DHCPNAK:
	case DHCPFORCERENEW:
	case DHCPLEASEUNASSIGNED:
	case DHCPLEASEUNKNOWN:
	case DHCPLEASEACTIVE:
		break;
	}
}


static int validate(struct dhcp_packet *packet, int rlen) {
	if (rlen < DHLEN) {
		_E(V_WARN,"packet too small: %d < %zu",rlen,DHLEN);
		return FAIL;
	}
	if (packet->p.eh.ether_type != htons(ETHERTYPE_IP)) {
		_E(V_WARN,"unexpected ether type: %x",packet->p.eh.ether_type);
		return FAIL;
	}
	if (packet->d.op != BOOTREQUEST) {
		_E(V_WARN,"unexpected dhcp op code on input: %d",packet->d.op);
		return FAIL;
	}
	if (packet->d.cookie != htonl(DHCP_COOKIE)) {
		_E(V_WARN,"option cookie mismatch %u != %u\n",
		   packet->d.cookie,htonl(DHCP_COOKIE));
		return FAIL;
	}
	if (ECMP(zero_mac,packet->d.chaddr) || 
			ECMP(bc_mac,packet->d.chaddr) ||
			ECMP(zero_mac,packet->p.eh.ether_shost) || 
			ECMP(zero_mac,packet->p.eh.ether_dhost)) {

		_E(V_WARN,"bad chaddr: " P_ETH " or ether host, ether_shost: " \
		   P_ETH " , ether_dhost: " P_ETH,
		P_ETHARG(packet->d.chaddr),
		P_ETHARG(packet->p.eh.ether_shost),
		P_ETHARG(packet->p.eh.ether_dhost));
		return FAIL;
	}
	return SUCCESS;
}
void clean_exit(int rc) {
	if (global.p_init_status == SUCCESS) 
		p_cleanup();
	if (global.pidfile_created)
		pid_file(global.pidfile,PID_DELETE);
	exit(rc);
}

void sig_handler(int sig) {
	safe_signal_handle(sig); /* 
							  * not so safe calling it here
							  */
}

void fill_eth_ip_udp_dh_hdr(struct dhcp_packet *request, 
							struct dhcp_reply *reply, 
							struct in_addr lease_ip) {
	ECOPY(global.my_mac,reply->p.eh.ether_shost);
	reply->p.eh.ether_type = htons(ETHERTYPE_IP);

	if (request->d.flags != 0)
		ECOPY(bc_mac,reply->p.eh.ether_dhost);
	else
		ECOPY(request->p.eh.ether_shost,reply->p.eh.ether_dhost);

	struct ip *ip = &reply->p.ip;
	ip->ip_v = 4;
	ip->ip_hl = sizeof(struct ip) >> 2;
	ip->ip_tos = IPTOS_LOWDELAY;
	ip->ip_len = htons(reply_packet_len(reply) - EHDR);
	ip->ip_id = 0;
	ip->ip_off = 0;
	ip->ip_ttl = 16;
	ip->ip_p = IPPROTO_UDP;
	ip->ip_sum = 0;
	ip->ip_src = global.siaddr;

	if (request->d.flags != 0) 
		ip->ip_dst.s_addr = INADDR_BROADCAST; 
	else 
		ip->ip_dst = lease_ip;
	
	struct udphdr *udp = &reply->p.udp;
	udp->uh_sport = htons(DHCPS);
	udp->uh_dport = htons(DHCPC);
	udp->uh_ulen  = htons(reply_packet_len(reply) - EHDR - sizeof(struct ip));
	udp->uh_sum = 0;
	reply->d.op = BOOTREPLY;
	reply->d.htype = 1; 
	reply->d.hlen = ETHER_ADDR_LEN;
	reply->d.hops = 0;
	reply->d.xid = request->d.xid;
	reply->d.secs = 0;
	reply->d.flags = request->d.flags;
	reply->d.ciaddr.s_addr = 0;
	reply->d.yiaddr = lease_ip;
	reply->d.giaddr = request->d.giaddr;
	COPY(request->d.chaddr,reply->d.chaddr,16);	
	reply->d.cookie = htonl(DHCP_COOKIE);

	/* just overwrite the ether dhost and ip->dst if we have relay agent */
	if (request->d.giaddr.s_addr) {
		ip->ip_dst = request->d.giaddr;
		ECOPY(request->p.eh.ether_shost,reply->p.eh.ether_dhost);
	}
}
static u16 reply_packet_len(struct dhcp_reply *r) {
	return sizeof(struct packet_header) + sizeof(struct dhcp_header) + r->o.pos;
}
static void discover(struct dhcp_packet *p,ssize_t rlen) {
	/* http://www.freesoft.org/CIE/RFC/2131/23.htm */
	struct dhcp_reply reply;
	if (gen_reply(p,&reply,DHCPDISCOVER,DHCPOFFER) == SUCCESS) {
		_E(V_NOTICE,"DHCPOFFER on %s to " P_ETH " via %s",
		   inet_ntoa(reply.d.yiaddr),
		   P_ETHARG(p->d.chaddr),global.ifname);
		pcap_inject(cap,&reply,reply_packet_len(&reply));
	} else {
		/* no free leases */
	}
}

static void request(struct dhcp_packet *p,ssize_t rlen) {
	/* http://www.freesoft.org/CIE/RFC/2131/24.htm */
	struct dhcp_reply reply;
	struct in_addr requested;
	u8 init_reboot;
	if (p->d.ciaddr.s_addr == 0) {
		requested.s_addr = o_find_u32(O_REQUEST_ADDR,p->options,rlen - DHLEN);
		if (requested.s_addr > 0) {
			/* DHCPREQUEST generated during INIT-REBOOT state */
			init_reboot = 1;
		}				
	} else {
		requested = p->d.ciaddr;
	}
	
	if (gen_reply(p,&reply,DHCPREQUEST,DHCPACK) == SUCCESS) {
		_E(V_NOTICE,"DHCPACK on %s to " P_ETH " via %s",
		   inet_ntoa(reply.d.yiaddr),P_ETHARG(reply.d.chaddr),
		   global.ifname);                                        
		/*
		 if (init_reboot) {
			 if (reply.d.yaddr.s_addr != requested.s_addr) {
				 reply.d.yaddr.s_addr = 0;
				 bzero(&reply.o, sizeof(reply.o));
				 reply.o.msg.msg_type = DHCPNAK;
			 }
		 }
		 */                                      
		pcap_inject(cap,&reply,reply_packet_len(&reply));
	} else {
		/* no free leases */
	}	
}

static void decline(struct dhcp_packet *p,ssize_t rlen) {
	/* http://www.freesoft.org/CIE/RFC/2131/25.htm */
}

static void release(struct dhcp_packet *p,ssize_t rlen) {
	/* http://www.freesoft.org/CIE/RFC/2131/26.htm */
	if (p_release(p->d.chaddr,p->d.ciaddr,p) == FAIL) {
		_E(V_WARN,P_ETH "tried to release %s, not leased to him", 
			P_ETHARG(p->d.chaddr),inet_ntoa(p->d.ciaddr));
	}
}

static void inform(struct dhcp_packet *p,ssize_t rlen) {
	/* http://www.freesoft.org/CIE/RFC/2131/27.htm */
	struct dhcp_reply reply;
	if (gen_reply(p,&reply,DHCPINFORM,DHCPACK) == SUCCESS) {
		_E(V_NOTICE,"DHCPACK to %s (" P_ETH ") via %s",
		   inet_ntoa(reply.d.yiaddr),P_ETHARG(p->d.chaddr),
		   global.ifname);
		pcap_inject(cap,&reply,reply_packet_len(&reply));

	}	
}

/*****************************************************************************
 * helper functions
 ****************************************************************************/
static int file_exist(char *file) {
	struct stat s;   
	return (stat(file,&s) == 0);
}

static void pid_file(char *file, int action) {
	if (action == PID_CREATE) {
		if (file_exist(file)) 
			SAYX("pid file:%s already exists",file);
		FILE *pfh = fopen(file,"w");
		if (!pfh) {
			perror("fopen");
			SAYX("cant create %s",file);
		}
		fprintf(pfh,"%d",getpid());
		fclose(pfh);
		global.pidfile_created = 1;
	} else {
		unlink(file);
	}
}

static void pcap_start(void) {
	struct bpf_program fp;
  	if ((cap = pcap_open_live(global.ifname, 1500, 0, 100, errbuf)) == NULL)
		SAYX("pcap_open_live(): %s", errbuf);
	if (pcap_compile(cap, &fp, PCAP_FILTER, 0, 0) < 0)
		SAYX("pcap_compile: %s", pcap_geterr(cap));
	if (pcap_setfilter(cap, &fp) < 0)
		SAYX("pcap_setfilter: %s", pcap_geterr(cap));
	if (pcap_loop(cap, 0, pcap_callback, NULL) < 0)
		SAYX("pcap_loop(%s): %s", global.ifname, pcap_geterr(cap));
}

void safe_signal_handle(int sig) {
	switch (sig) {
		case SIGUSR1:
		case SIGUSR2:
		case SIGALRM:
			p_signal(sig);
			alarm(SIGNAL_INTERVAL);
			break;
		case SIGTERM:
		case SIGQUIT:
		case SIGINT:
			clean_exit(EXIT_SUCCESS);
			break;
	}
}

static int setmac(u8 *dest,char *ifname){
	struct ifaddrs *ifas, *ifa;
	getifaddrs (&ifas);
	for (ifa = ifas; ifa != NULL; ifa = ifa->ifa_next) {
#define SDL ((struct sockaddr_dl *)ifa->ifa_addr)
	    if (strcmp (ifa->ifa_name, ifname)
			|| SDL->sdl_family != AF_LINK
			|| SDL->sdl_alen != ETHER_ADDR_LEN)
			continue;
	    ECOPY(SDL->sdl_data + SDL->sdl_nlen, dest);
	    return SUCCESS;
	}
	return FAIL;
#undef SDL	
}

static void usage(void) {
	printf("dhcpd [-fvh -p pid_file] -i ifname -s siaddr\n");
	printf("\t-i interface (required)\n");
	printf("\t-s siaddr (required)\n");
	printf("\t---------\n");
	printf("\t-f do not demonize, log to stderr instead to syslog\n");
	printf("\t-v verbose level (default:0) can be used multiple times (-vvv)\n");
	printf("\t-p pid_file (default: %s)",PID_FILE);
	printf("\t-h prints this info\n");
}

static void signal_init(void) {
	signal(SIGALRM, sig_handler);
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGINT, sig_handler);
	alarm(SIGNAL_INTERVAL);
}

static void socket_bind(void) {
	/* so we dont need udp blackhole */
	return;
//	int s;
//	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
//		SAYX("cant open socket");		
//	
//	struct sockaddr_in si;
//	bzero(&si,sizeof(si));
//	si.sin_family = AF_INET;
//	si.sin_port = htons(DHCPS);
//	si.sin_addr.s_addr = htonl(INADDR_ANY);
//	if (bind(s, (struct sockaddr *) &si, sizeof(si)) < 0) 
//		SAYX("cant bind on port %d",DHCPS);
	
}
