#include "dhcpd.h"
extern struct __global global;
static void o_pad(struct options *o);
int o_add(struct options *o, u8 type, u8 len, void *value) {
	
	u16 real_len = len + 2; /* data len + 1 byte type and 1 byte len */
	if (o->pos + real_len > MAX_OPTIONS_LEN) {
		_E(V_CRY,"ran out of option space: %d + %u > %u",
			o->pos,real_len, MAX_OPTIONS_LEN);
		return FAIL;
	}
	o->data[o->pos++] = type;
	o->data[o->pos++] = len;
	COPY((u8 *) value,&o->data[o->pos],len);
	o->pos += real_len;
	return SUCCESS;
}
int o_add_u64(struct options *o, u8 type, u64 val) {
	return o_add(o,type,8,&val);
}
int o_add_u32(struct options *o, u8 type, u32 val) {
	return o_add(o,type,4,&val);
}
int o_add_u16(struct options *o, u8 type, u16 val) {
	return o_add(o,type,2,&val);
}
int o_add_u8(struct options *o, u8 type, u8 val) {
	return o_add(o,type,1,&val);
}

int o_end(struct options *o) {
	if (o->pos < MAX_OPTIONS_LEN) {
		o->data[o->pos++] = O_END;
		o_pad(o);
		return SUCCESS;
	}
	return FAIL;
}
static void o_pad(struct options *o) {
	int pad = MIN_OPTIONS_LEN - o->pos;
	if (pad > 0 && o->pos + pad < MAX_OPTIONS_LEN) {
		ZERO(&o->data[o->pos],pad);
		o->pos += pad;
	}
}
int o_find(u8 option_code,u8 min_len,u8 *s,int avail,struct tlv *dest) {
	bzero(dest,sizeof(*dest));
#define CODE	s[0]
#define LEN		s[1]
#define VAL		s[2]
#define METASIZ 2
#define MIN_OPT_LEN METASIZ + 1 /* 1 byte code, 1 byte len 1 byte data */
	do {
		if (avail < MIN_OPT_LEN)
			goto bad;
		if (CODE == option_code && LEN >= min_len) {
			dest->type = option_code;
			dest->len = LEN;
			dest->value = s+METASIZ;
			return SUCCESS;
		}
		s += METASIZ + LEN;
		avail -= METASIZ + LEN;
	} while (avail > 0 && CODE != 0 && CODE != O_END);
#undef CODE
#undef LEN
#undef VAL
#undef METASIZ    
#undef MIN_OPT_LEN
bad:
    return FAIL;
}

u8 o_find_u8(u8 option_code,u8 *s,int avail) {
	struct tlv t;
	o_find(option_code,1,s,avail,&t);
	return *(u8 *)t.value;
}
u16 o_find_u16(u8 option_code,u8 *s,int avail) {
	struct tlv t;
	o_find(option_code,2,s,avail,&t);
	return *(u16 *)t.value;
}

u32 o_find_u32(u8 option_code,u8 *s,int avail) {
	struct tlv t;
	o_find(option_code,4,s,avail,&t);
	return *(u32 *)t.value;
}
u64 o_find_u64(u8 option_code,u8 *s,int avail) {
	struct tlv t;
	o_find(option_code,8,s,avail,&t);
	return *(u64 *)t.value;
}
