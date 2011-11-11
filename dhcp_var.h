/* http://www.iana.org/assignments/bootp-dhcp-parameters/bootp-dhcp-parameters.xml */
#define DHCPDISCOVER	1		/* http://tools.ietf.org/html/rfc2132 */
#define DHCPOFFER		2		/* http://tools.ietf.org/html/rfc2132 */
#define DHCPREQUEST		3		/* http://tools.ietf.org/html/rfc2132 */
#define DHCPDECLINE		4		/* http://tools.ietf.org/html/rfc2132 */
#define DHCPACK			5		/* http://tools.ietf.org/html/rfc2132 */
#define DHCPNAK			6		/* http://tools.ietf.org/html/rfc2132 */
#define DHCPRELEASE		7		/* http://tools.ietf.org/html/rfc2132 */
#define DHCPINFORM		8		/* http://tools.ietf.org/html/rfc2132 */
#define DHCPFORCERENEW	9		/* http://tools.ietf.org/html/rfc3203 */
#define DHCPLEASEQUERY	10		/* http://tools.ietf.org/html/rfc4388 */
#define DHCPLEASEUNASSIGNED	11	/* http://tools.ietf.org/html/rfc4388 */
#define DHCPLEASEUNKNOWN	12	/* http://tools.ietf.org/html/rfc4388 */
#define	DHCPLEASEACTIVE		13	/* http://tools.ietf.org/html/rfc4388 */
#define DHCP_MAX_MTU 1500
#define BOOTREQUEST		1
#define BOOTREPLY		2
#define DHLEN (sizeof(struct packet_header) + sizeof(struct dhcp_header)) + 1
#define DHCP_COOKIE		0x63825363
#define DNS_COUNT		2
#define MAX_ID_LEN		20
#define	DHCPS			67
#define	DHCPC			68
#define PCAP_FILTER		"udp and dst port 67 and src port 68"
#ifndef __packed
 #define __packed __attribute__ ((__packed__))
#endif

struct packet_header {
        struct ether_header eh;
        struct ip ip;
        struct udphdr udp;
} __packed;
struct dhcp_header {
  	u8  op;				/* 0: Message opcode/type */
	u8  htype;			/* 1: Hardware addr type (net/if_types.h) */
	u8  hlen;			/* 2: Hardware addr length */
	u8  hops;			/* 3: Number of relay agent hops from client */
	u32 xid;			/* 4: Transaction ID */
	u16 secs;			/* 8: Seconds since client started looking */
	u16 flags;			/* 10: Flag bits */
	struct in_addr ciaddr;		/* 12: Client IP address (if already in use) */
	struct in_addr yiaddr;		/* 16: Client IP address */
	struct in_addr siaddr;		/* 20: IP address of next server to talk to */
	struct in_addr giaddr;		/* 24: DHCP relay agent IP address */
	u8 chaddr [16];			/* 28: Client hardware address */
	u8 sname[64];			/* 44: Server name */
	u8 file[128];			/* 108: Boot filename */
	u32 cookie;				/* XXX: does not belong here */
} __packed;
struct dhcp_packet {
        struct packet_header p;
        struct dhcp_header d;
		u8 options[1];
} __packed;

struct tlv {
	u8 type;
	u8 len;
	u8 *value;
};
/* 
 * reply packet, filled by lease plugin
 * (must fill everything, there is an example in export.h)
 * http://www.iana.org/assignments/bootp-dhcp-parameters/bootp-dhcp-parameters.xml
 */
#define MAX_OPTIONS_LEN 1175	/* DHCP_MAX_MTU - sizeof(struct packet_header) - sizeof(struct dhcp_packet) ;*/
#define MIN_OPTIONS_LEN 60		/* 300 - sizeof(dhcp_header); */
struct options {
	u8 data[MAX_OPTIONS_LEN];
	u16 pos;
} __packed;

struct dhcp_reply {
	struct packet_header p;
	struct dhcp_header d;
	struct options o;
} __packed;

struct __global {
	char id[MAX_ID_LEN];
	u8 my_mac[ETHER_ADDR_LEN];
	struct in_addr siaddr;
	u8 stderr_log;
	int verbose;
	char *ifname;
	char *pidfile;
	u8 pidfile_created;
	int p_init_status;
};

#define O_PAD			0
#define O_SUBNET		1
#define O_ROUTER		3
#define O_DNS			6
#define O_BROADCAST		28
#define O_REQUEST_ADDR	50
#define O_LEASE_TIME	51
#define O_MSGTYPE		53
#define O_SRV_ID		54
#define O_MSZ			57
#define O_RELAYAGENT	82
#define O_END			255
