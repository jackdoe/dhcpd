#ifndef _static_h
#define _static_h
#define DB_FILE "/tmp/bdhcpd-static.db"
/* stored for each mac in the db file */
struct lease {
	struct in_addr ip;
	struct in_addr subnet;
	struct in_addr dns[DNS_COUNT];
	struct in_addr gw;
	struct in_addr broadcast;
	u32 lease_time;
} __packed;
#endif
