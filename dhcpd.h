#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <net/bpf.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_var.h>
#include <netinet/if_ether.h>
#include <sys/param.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/if_dl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <err.h>
#include <syslog.h>
#include <stdarg.h>
#include <signal.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <err.h>
#include <pcap.h>
#include <sys/cdefs.h>
#include <sys/stat.h>

typedef u_char u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#include "dhcp_var.h"

#define COPY(src,dst,len) bcopy(src,dst,len)
#define ZERO(a,len) bzero(a,len)
#define ECOPY(src,dst) COPY(src,dst,ETHER_ADDR_LEN)
#define ECMP(a,b) (bcmp(a,b,ETHER_ADDR_LEN) == 0)
#define EZERO(a) ZERO(a,ETHER_ADDR_LEN)
#define P_ETH "%02x:%02x:%02x:%02x:%02x:%02x"
#define P_ETHARG(addr) (u8) addr[0],(u8) addr[1],(u8) addr[2],(u8) addr[3],(u8) addr[4],(u8) addr[5]

#define V_INFO		4
#define V_NOTICE	3
#define V_WARN		2
#define V_ERR		1
#define V_CRY		0
#define SYSLOG_LEVEL	LOG_INFO
#define MAX_PIDFILE_LEN 256
#define PID_FILE	"/var/run/bdhcpd.pid"

#ifdef DEBUG
#define _FORMAT(fmt,arg...) fmt " [%s():%s:%d]\n", ##arg,__func__,__FILE__,__LINE__
#else
#define _FORMAT(fmt,arg...) fmt "\n",##arg
#endif

#define _E(__level,fmt, arg...)								\
do {														\
	if (__level == V_CRY) {									\
		syslog(SYSLOG_LEVEL,_FORMAT(fmt,##arg));			\
		fprintf(stderr,_FORMAT(fmt,##arg));					\
	} else {												\
	  if (global.verbose >= __level) {						\
		if (global.stderr_log == 0)							\
		  syslog(SYSLOG_LEVEL,_FORMAT(fmt,##arg));			\
		else												\
		  fprintf(stderr,_FORMAT(fmt,##arg));				\
	  }														\
	}														\
} while(0);
/* say and exit */
#define SAYX(fmt,arg...)		\
do {							\
	_E(V_CRY,fmt,##arg);		\
	clean_exit(EXIT_FAILURE);	\
} while (0);

#ifndef EHTER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif
#define EHDR sizeof(struct ether_header)
#define MAX_IFNAME_LEN 20
#define COMMIT 1
#define EXPIRE 2
#define RELEASE EXPIRE
#define SUCCESS 1
#define FAIL 0
#define PID_CREATE 1
#define PID_DELETE 0
#define IP_ZERO(ip) (ip.s_addr == 0)
extern u8 zero_mac[ETHER_ADDR_LEN];
extern u8 bc_mac[ETHER_ADDR_LEN];
extern char *msg2string[];
void expiry(u8 *mac, struct in_addr ip); /* so plugins can tell us that a lease has expired */
void commit(u8 *mac, struct in_addr ip,u32 lease_time);
void fill_eth_ip_udp_dh_hdr(struct dhcp_packet *request, struct dhcp_reply *reply, struct in_addr lease_ip);

int o_find(u8 option_code,u8 min_len,u8 *s,int avail,struct tlv *dest);
u8 o_find_u8(u8 option_code,u8 *s,int avail);
u16 o_find_u16(u8 option_code,u8 *s,int avail);
u32 o_find_u32(u8 option_code,u8 *s,int avail);
u64 o_find_u64(u8 option_code,u8 *s,int avail);

int	o_add(struct options *o, u8 type, u8 len, void *value);
int	o_add_u64(struct options *o, u8 type, u64 val);
int	o_add_u32(struct options *o, u8 type, u32 val);
int	o_add_u16(struct options *o, u8 type, u16 val);
int	o_add_u8(struct options *o, u8 type, u8 val);
int o_end(struct options *o);

/* 
 * clean_exit() can be called from plugin
 * MUST NOT be called from p_clean(), or infinite recursion will occur.
 */
void clean_exit(int rc);
