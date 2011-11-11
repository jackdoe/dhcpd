#DECISION= dummy
DECISION= mysql-static

CC= clang
CFLAGS= -Wall -g3 -DDEBUG
include decision-maker/$(DECISION)/required-libs

all:
	$(CC) $(CFLAGS) $(LIBS) dhcpd.c decision-maker/$(DECISION)/$(DECISION).c options.c ip-util.c -o dhcpd -l pcap 
clean:
	rm -f dhcpd

	
