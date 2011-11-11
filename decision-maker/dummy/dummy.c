#include "../../dhcpd.h"
#include "../export.h"
extern struct __global global;
int p_reply(struct dhcp_reply *reply_packet,
				struct dhcp_packet *request_packet,
				u8 reply_msg_type, u8 request_msg_type) {
        _E(V_ERR,"cant get lease from dummy plugin");
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

}
void p_init(int *init_status) {
	*init_status = SUCCESS;
    _E(V_CRY,"dummy plugin init");
}
void p_cleanup(void) {
    _E(V_CRY,"dummy plugin cleanup");
}
