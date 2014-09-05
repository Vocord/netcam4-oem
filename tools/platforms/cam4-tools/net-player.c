/*\
 *  
 *  Copyright (C) 2004-2014 VOCORD, Inc. <info@vocord.com>
 *
 * This file is part of the P3SS API/ABI/VERIFICATION system.
 *
 * The P3SS API/ABI/VERIFICATION system is free software; you can
 * redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The part of the P3SS API/ABI/VERIFICATION system
 * is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with the part of the P3SS API/ABI/VERIFICATION system;
 * if not, see <http://www.gnu.org/licenses/>.
 *
 *
 *       Component for conform portion of the system.
\*/

#define _GNU_SOURCE
/*\
 *	Description:	Capture module designed for gazering network traffic
 *			data 
 * 
 * 
\*/
#include <inttypes.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <fcntl.h>

#include <netinet/ip.h>

#include <timers/time.h>


#include <abi/ip-video-raw.h>
#include <net/cap-files.h>

#define TRACE_PRIVATE_PREFIX    1
#include <trace.h>
#undef  TRACE_LEVEL
#define TRACE_LEVEL 4

typedef struct {
	char	a[6];
} MAC;

static MAC	bMAC = {
  .a = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
};

typedef struct {
	uint8_t	dst[6];
	uint8_t	src[6];
	uint16_t type;
	uint8_t	data[0];
} __attribute__ ((packed)) ethernet_frame;

typedef struct {
	uint8_t	dst[6];
	uint8_t	src[6];
	uint16_t type;
	uint16_t no_matter;
	uint16_t type2;
	uint8_t	data[0];
} __attribute__ ((packed))  ieee802_1q_frame;

typedef struct {
#if(BYTE_ORDER == LITTLE_ENDIAN)
	uint32_t ttl:8;
	uint32_t bottom:1;
	uint32_t exp:3;
	uint32_t label:20;
#else
	uint32_t label:20;
	uint32_t exp:3;
	uint32_t bottom:1;
	uint8_t ttl;
#endif
} __attribute__ ((packed)) mpls_hdr;

typedef void (*sighandler_t)(int);

static char* trace_prefix = "CAM4-DR:\t";

static sighandler_t	old_int;

FILE	*I = NULL;


/* var definition */
static size_t		snaplen		= 65534;

static struct timeval	tv_last		= { };
static int		debug		= 0;

static uint8_t		DATA[65536];
static ethernet_frame	*pdata		= (void*)DATA;		/* packet data		*/

static long		cycle		= 0;

static int		bcast		= 0;
static int		fd		= -1;
static uint64_t		j		= 0;

static int		capture_enabled = 0;
static int		capture_event	= 0;	/* no event */

struct timeval	tv_prev;	/* timestamp of last captured packet */

int rpo_nlen = 0;

/* dynamic functions initializers */
static void	player_no_delay(void);
static void	player_delay_start(void);


static int	read_packet_tcpdump(int fd);
static int	read_packet_own(int fd);

/* dynamic functions */
static void	(*do_delay)(void) = &player_no_delay;
static int	(*read_packet_hdr)(int) = &read_packet_own;

//static char s_normal[]	= { 0x1b, '[', '0', ';', '3', '9', 'm', '\0' };


typedef struct {
  uint8_t		a[8];
  uint8_t		b[8];
  uint16_t		snap_len;
  uint8_t		c[2];
  uint8_t		d[4];
}  __attribute__((packed))  tcpdump_file_header;

static tcpdump_file_header tcpdump_preambula = {
   .a = {0xd4, 0xc3, 0xb2, 0xa1, 0x02, 0x00, 0x04, 0x00},
   .b = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
   .snap_len = 65534,
   .c = {0x00, 0x00},
   .d = {0x01, 0x00, 0x00, 0x00}
};

tcpdump_packet_header_t header_tcpdump;
tcpdump_packet_header_t *hdt_1;
#if 0
static int dump_data_empty(int fd, void *data, uint32_t len)
{
	return 0;
}
static int dump_data_own(int fd, void *data, uint32_t len)
{
	header_tcpdump.header_own.tv_last.tv_sec = (int32_t)tv_last.tv_sec;
	header_tcpdump.header_own.tv_last.tv_usec = (int32_t)tv_last.tv_usec;
	header_tcpdump.header_own.len = len;
	
	int res = write (fd, &header_tcpdump.header_own, sizeof (header_tcpdump.header_own));
	if(res<0) {
		ETRACE("When self dumping");
		return -1;
	}
	return write (fd, pdata, len);
}
static int	(*dump_data)(int, void*, uint32_t) = &dump_data_own;
#endif

static int cap_stop_async (int event)
{
	capture_enabled = 0;
	capture_event = event;

	return 0;
};

static void catch_sig_interrupt(int sig_num)
{
    cap_stop_async(1);

    if(old_int != SIG_DFL && old_int != SIG_IGN)
	    (*old_int)(sig_num);
}

extern void set_act_value(unsigned long value);
    
void set_act_value(unsigned long value)
{
}

extern int s_read(int fd, char *buf, size_t bytes_to_read);

int s_read(int fd, char *buf, size_t bytes_to_read)
{
	return 0;
}
#if PROTOCOL_DEBUG_LEVEL > 3
static void show_mac (const uint8_t * mac)
{
	TRACE(2, "%02x:%02x:%02x:%02x:%02x:%02x ", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
void show_hex(int size, const uint8_t * data, int pos)
{
	int i;
	for(i = pos; i < size + pos; i++, data++) {
		if((i & 31) == 0) {
			TRACE (2, "\n");
		}
		if((i & 31) == 16) {
			TRACE (2, " ");
		}

		if((i & 15) == 7) {
			TRACE (2, "%02x-", *data);
		} else	{
			TRACE (2, "%02x ", *data);
		}
	}
}
#endif

typedef struct {
	video_frame_raw_hdr_t	lfh;
	video_frame_raw_t	lfd;

	uint_fast32_t		offs;
	uint_fast32_t		max_size;

	unsigned		seq;
	unsigned		flags;
	unsigned		sa;
	unsigned		se;

	int			fd;
	int			good;
	const char		*prefix;
} raw_ctx_t;

void process_nc4_fd(
	raw_ctx_t			*ctx,
	video_frame_raw_t		*fd
)
{

	fd->lid		= ntohl(fd->lid);

	fd->size	= ntohs(fd->size);

	fd->x_dim	= ntohs(fd->x_dim);
	fd->y_dim	= ntohs(fd->y_dim);

	fd->offs	= ntohl(fd->offs);

	uint32_t	offs  = fd->offs & 0x0fffffff;

	unsigned	sa = (fd->offs>>28)*2+8;

	TRACE(0, "FD %5d@%8d %5ux%5u %dbpp\n", fd->size, offs, fd->x_dim, fd->y_dim, sa);

	if(fd->size > 16384) {
//		ctx->good = 0;
		TRACE(0, "WARN size too big: %u\n",
		    fd->size
		);
	}

	if(offs + fd->size > ctx->max_size) {
		ctx->good = 0;
		TRACE(0, "WARN size+offs too big: %u\n",
		    offs + fd->size
		);
	}

	if(ctx->seq != fd->fseq) {
		ctx->good = 0;
		TRACE(0, "WARN SEQ invalid: %3u instead %3u\n",
		    fd->fseq,
		    ctx->seq
		);

		ctx->seq = fd->fseq;
	}

	if(offs != ctx->offs) {
		TRACE(0, "WARN fd missed: %8u octets @ [%8u-%8u)\n",
		    offs - ctx->offs,
		    ctx->offs,
		    offs
		);
	}

	ctx->offs	= offs + fd->size;
	ctx->lfd	= *fd;

	ctx->flags	= fd->flags;
	ctx->sa		= sa;

	if(ctx->good && (ctx->fd >= 0)) {
		ssize_t res = pwrite(ctx->fd, (const void*)(fd+1), fd->size, offs);
		if(res<0)
			ETRACE("Dumping RAW");
	}
}

void process_nc4_fh(
	raw_ctx_t			*ctx,
	video_frame_raw_hdr_t		*fh
)
{
	if(ctx->fd >= 0)
		close(ctx->fd);

	fh->lid		= ntohl(fh->lid);
	fh->fsize	= ntohl(fh->fsize);
        fh->osize	= ntohl(fh->osize);

	fh->fseq	= ntohl(fh->fseq);

	fh->x_dim	= ntohs(fh->x_dim);
	fh->y_dim	= ntohs(fh->y_dim);

	uint32_t	fsize = fh->fsize & 0x0fffffff;
	uint32_t	osize = fh->osize & 0x0fffffff;

	unsigned	sa = (fh->fsize>>28)*2+8;
	unsigned	se = (fh->osize>>28)*2+8;

	TRACE(0, "FH LID:%08x SEQ:%08x w*h:%5u*%5u fsz:%8u osz:%5u se:%2u sa:%2u flags:%02x D:%+d\n",
		fh->lid,
		fh->fseq,
		fh->x_dim,
		fh->y_dim,
		fsize,
		osize,
		se,
		sa,
		ctx->flags,
		fh->fseq - ctx->lfh.fseq
	);

	ctx->lfh	= *fh;
	ctx->offs	= 0;
	ctx->seq	= fh->fseq & 0xff;
	ctx->max_size	= fsize + osize;

	if(!ctx->prefix)
	    return;

	char	name[1024];

	snprintf(name, sizeof(name)-1, "%s.%08x.%dx%d.%dbit.raw",
		ctx->prefix,
		fh->fseq,
		fh->x_dim,
		fh->y_dim,
		sa
	);

	name[sizeof(name)-1] = 0;

	ctx->fd = open(name, O_CREAT|O_RDWR|O_TRUNC, 0666);
	ctx->good = 1;
}

static raw_ctx_t		ctx = { .fd = -1 };

void process_nc4(void* nc4, int pkt_len)
{
	video_frame_raw_hdr_t		*fh = nc4;
	video_frame_raw_t		*fd = nc4;

	uint32_t			lid = ntohl(fh->lid);

	if(lid & 0x80000000)
		process_nc4_fh(&ctx, fh);
	else
		process_nc4_fd(&ctx, fd);
}

typedef struct {
	int (*func) (struct iphdr *);
	char *name;
} proto_cell;

static proto_cell proto[256] = {
	[0]		= {},
#if 0
	[IPPROTO_ICMP]	= {NULL, "ICMP"},
	[IPPROTO_IGMP]	= {NULL, "IGMP"},
	[3]		= {NULL, "GGP"},
	[IPPROTO_IPIP]	= {NULL, "IPIP"},
	[IPPROTO_TCP]	= {NULL, "TCP"},
	[IPPROTO_EGP]	= {NULL, "EGP"},
	[IPPROTO_PUP]	= {NULL, "PUP"},
	[IPPROTO_UDP]	= {NULL, "UDP"},
	[IPPROTO_IDP]	= {NULL, "IDP"},
	[IPPROTO_IPV6]	= {NULL, "IPv6-in-IPv4"},
	[IPPROTO_RSVP]	= {NULL, "RSVP"},
	[IPPROTO_GRE]	= {NULL, "GRE"},
	[IPPROTO_ESP]	= {NULL, "EncSecurPayload"},
	[IPPROTO_AH]	= {NULL, "AuthHeader"},
	[IPPROTO_PIM]	= {NULL, "ProtIndepMulticast"},
	[IPPROTO_COMP]	= {NULL, "CompHeader"},
	[253]		= {NULL, "NC4-RAW"},
#endif
	[IPPROTO_RAW]	= {NULL, "RAW IP"},
};


void process_ip(uint8_t* ip, int pkt_len)
{
	struct iphdr* iph = (typeof(iph))ip;
	if(pkt_len < sizeof(struct iphdr))
		return;

	iph->daddr = ntohl(iph->daddr);
	iph->saddr = ntohl(iph->saddr);

	if(ntohs(iph->frag_off) & 0x3fff) {
		TRACEP(0, "IPf|%5u|\n", ntohs(iph->frag_off));
	}

	if(proto[iph->protocol].name) {
		TRACE(4, " %4s", proto[iph->protocol].name);
	}

	if(pkt_len < ntohs(iph->tot_len)) {
		TRACEP(0, "WARN: ip length=[%5u], got packet length=[%5u]. skipping it.\n", ntohs(iph->tot_len), pkt_len);
		return;
	}

	if(ntohs(iph->tot_len) < sizeof(struct iphdr)) {
		TRACEP(0, "WARN: too small iph len=%5u, skip packet\n", iph->tot_len);
		return;
	}

	if(iph->protocol == 253) {
		TRACEP(2, "IP|%5u ", ntohs(iph->tot_len));
		process_nc4(ip + 4*iph->ihl, ntohs(iph->tot_len));
	}

	return;
}

void process_rarp (const unsigned char *data)
{
	TRACEP (2, "[RARP]\n");
}
void process_arp (const unsigned char *data)
{
	TRACEP (2, "[ARP]\n");
}

void process_ethernet (ethernet_frame* ether, int pkt_len)	/* !!!! this proc may accesses whole frame !!!! */
{
	uint16_t		type	= ntohs(ether->type);
	ieee802_1q_frame	*i8021q;
	uint16_t		type2;

#if PROTOCOL_DEBUG_LEVEL > 3
	show_mac(ether->dst);
	show_mac(ether->src);
#endif
	switch(type) {
		/* Ethernet_II (RFC894) interesting frames */
		case 0x0800:
			return process_ip(ether->data, pkt_len-14);

		case 0x0806:
			return process_arp(ether->data);

		case 0x0835:
			process_rarp(ether->data);
			break;

			/* Ethernet_802.1Q frames */
		case 0x8100:
			i8021q	= (ieee802_1q_frame*)ether;
			type2	= ntohs(i8021q->type2);
			if(type2 == 0x0800)
				return process_ip(i8021q->data, pkt_len-18);

			if(type2 == 0x0806)
				return process_arp(i8021q->data);
			break;

		default:
			/* NON Ethernet_802.3 frame */
			TRACEP(2, "UNKNOWN ETH FRAME {%04u}\n", type);
			break;
	}
}

static void player_no_delay(void)
{
	return;
}

/* delay packet delivery */
static void player_delay(void)
{
	struct timeval	tv = tv_last;  /* timestamp of last captured packet */
	struct timespec	ts;

	int res = 1;
		
	if(sub_timer(&tv, tv_prev)) {
		goto done;
	}
	  
	ts.tv_sec  = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000;

	while(res && ((ts.tv_sec>0) || (ts.tv_nsec>0))) {
		TRACEPNF(11, "sleep for: %6ld.%09ld\n", ts.tv_sec, ts.tv_nsec);
		res = nanosleep(&ts, &ts);
	};

done:
	tv_prev = tv_last;

	return;
}

static void player_delay_start(void)
{
	TRACEP(0, "Playing in real time Scale\n");
	tv_prev = tv_last;
	do_delay = &player_delay;

	return;
}

ssize_t sniff_safe_read(int fd, char *buf, size_t bytes_to_read)
{
	ssize_t r;
	ssize_t	nread = 0;

	while(nread < bytes_to_read) {
		r = read(fd, (char*)(buf + nread), bytes_to_read - nread);

		if(r > 0) {
			nread += r;
			continue;
		}

		if(!r)
			break;

		if(errno == EAGAIN)
			break;
	
		if(errno != EINTR) {
			nread = -1;
			break;
		}

		break;
	}

	return nread;
}


/* ([tv] [recorded_len] [data]){n times} */
static int read_packet_own_run(int fd)
{
	int res; 

	res = sniff_safe_read(fd, (char*)((char*)&header_tcpdump.header_own + rpo_nlen), sizeof(header_tcpdump.header_own) - rpo_nlen);
	if(res < 0) {
		TRACEP(0, "ERR: on read tcpdump packet header: [%d]\n", errno);
		rpo_nlen = 0;
		return -1;
	}

	rpo_nlen += res;
	TRACEP(10, "rp res:[%d] of [%d], rpo_nlen:[%d]\n", res, sizeof(header_tcpdump.header_own), rpo_nlen);

	if(rpo_nlen == sizeof(header_tcpdump.header_own)) {
		rpo_nlen = 0;
		return 0;
	}
	
	return 1;
}

static int read_packet_own(int fd)
{
	TRACEP (0, "using own format for Network Traffic\n");
	
	read_packet_hdr = &read_packet_own_run;
	return read_packet_hdr(fd);
}

/* {PREAMBULE} ([tv] [recorded_len] [len] [data]){n times} */
static int read_packet_tcpdump_run(int fd)
{
	int res; 
	static int nlen = 0;

	res = sniff_safe_read(fd, (char*)((char*)&header_tcpdump + nlen), sizeof(header_tcpdump) - nlen);
	if(res < 0) {
		TRACEP(0, "ERR: on read tcpdump packet header: [%d]\n", errno);
		nlen = 0;
		return -1;
	}

	nlen += res;
	TRACEP(10, "rp res:[%d] of [%d], nlen:[%d]\n", res, sizeof (header_tcpdump), nlen);

	if(nlen == sizeof(header_tcpdump)) {
		nlen = 0;
		return 0;
	}

	return 1;
}

static int read_packet_tcpdump(int fd)
{
	int res;

	TRACEP (0, "using tcpdump format  for Network Traffic\n");
	res = sniff_safe_read(fd, (char*)pdata, sizeof(tcpdump_preambula));
	TRACEP(11, "tcp preambule read: %d bytes\n", res);

	if(res && (res != sizeof(tcpdump_preambula))) {
		return -1;
	}

	read_packet_hdr = &read_packet_tcpdump_run;

	res = read_packet_hdr(fd);

	return res;
}

static inline int play_packet(int fd)
{
	int		resr;
	uint32_t	len = 0;


	resr = (*read_packet_hdr)(fd);

	len = header_tcpdump.header_own.len;
	tv_last.tv_sec = header_tcpdump.header_own.tv_last.tv_sec;
	tv_last.tv_usec = header_tcpdump.header_own.tv_last.tv_usec;

	TRACEP(11, "%s:get header resulted in: %d\n", __func__, resr);
	TRACEP(12, "\t:get len resulted in: %08x\n", len);
	TRACEP(12, "\t:get sl resulted in: %08x\n",  header_tcpdump.len);

	if(len > snaplen) {
		TRACEP(0, "%s:input file corrupted:\n", __func__);
		TRACEP(0, "\t[%8Lu] Invalid packet len: %u = 0x%08x\n", j, len, len);
		return 1;
	}

	if(resr < 0) {	
		TRACEP (0, "%s when reading from file\n", strerror(errno));

		if(!resr) {
			return -2;	/* EOF */
		}

		return -1;		/* invalid header */
	}

	resr = read (fd, pdata, len);

	(*do_delay)();
	j++;

	if(bcast) {
		((MAC*)pdata)[0] = bMAC;
	}

	process_ethernet(pdata, len);
#if 0
	for(i = 0; i < devs; i++) {
		resw = write(fds[i].fd, pdata, len);

		if(resw != len) {
			TRACEP (0, "%s when writting data to network\n",
			     strerror(errno));
			return 1;
		} 
	}
#endif
	return (resr <= 0) || !capture_enabled;
}

static void
show_help_msg_player (char *name)
{
	TRACEP(0,"help_msg...\n");
	TRACE (0, "This is a network traffic player application.\n");
	TRACE (0, "Usage: %s ", name);
	TRACE (0, "[-t] [-r] [-f name] [-c n] [-b]\n");
	TRACE (0, "Options:\n");
//	TRACE (0, "  -q   | --quiet	do not write any data decode only\n");
	TRACE (0, "  -c n | --cycle n	cycle data for n times,\n");
	TRACE (0, "                  	if n<0 do infinite cycling\n");
	TRACE (0, "  -d n | --debug n	debug level n\n");
	TRACE (0, "  -f n | --file name	play data from file name\n");
	TRACE (0, "  -h   | --help	this text\n");
	TRACE (0, "  -r   | --realtime	write data in realtime\n");
	TRACE (0, "  -t   | --tcpdump	read write tcpdump compatible data\n");
	exit (1);
}

int play_start (int argc, char **argp, char **argv)
{
	char	*name = NULL;

	/* parse options */
	for(;;) {
		int c;
		if(-1 == (c = getopt (argc, argp, "bi:f:hc:rtp:"))) {
			TRACEP(0,"no more opts?\n");
			break;
		}

		switch (c) {
		    case 0:
			/* long option */
			break;

		    case 'd':
			debug = strtol(optarg, NULL, 0);
			break;

		    case 'p':
			ctx.prefix = optarg;
			TRACEP(0,"\tdata dump file name is: \"%s\"\n", name);
			break;

		    case 'f':
			name = optarg;
			TRACEP(0,"\tNetwork traffic file name is: \"%s\"\n", name);
			break;

		    case 'c':
			cycle = strtol(optarg, NULL, 0);
			break;
			    
		    case 'r':
			do_delay = &player_delay_start;
			
			break;

		    case 't':
			read_packet_hdr = &read_packet_tcpdump_run;
			
			break;

		    case 'h':
		    default:
			show_help_msg_player (argp[0]);
		}
	}

	TRACEP(0,"activating devices\n");

	signal (SIGTERM, catch_sig_interrupt);	/* SIGTERM handler */
	signal (SIGINT, catch_sig_interrupt);	/* SIGINT handler */
	j = 0;
	capture_enabled = 1;

	do {
		TRACEP (1, "open file: %s for reading\n", name);
		fd = open (name, O_RDONLY | O_LARGEFILE);
		if(fd < 0) {
			TRACEP (0,
			     "%s when open traffic file for reading: %s\n",
			     strerror(errno),
			     name
			);

			break;
		}

		if(read_packet_hdr == &read_packet_tcpdump_run)
			read_packet_hdr = &read_packet_tcpdump;
		else 
			read_packet_hdr = &read_packet_own;

		if(do_delay == &player_delay)
			do_delay = &player_delay_start;

		if(fd > 0)
			while(capture_enabled && !play_packet(fd)) ;

		if(cycle>0)
			cycle--;
		
		close(fd);

		TRACE(2, "\n");
		TRACEP (2, "Traffic Plaing finished\n");
		TRACEP (2, "Packets transmeeted: %Lu\n", j);
		TRACEP (2, "Iiterations remaining: %ld\n", cycle);

	} while(capture_enabled && cycle);

	if(ctx.fd >= 0)
		close(ctx.fd);

	return 0;
};

int main(int argc, char **argp, char **argv)
{

    I = stdout;

    old_int = signal(SIGINT, catch_sig_interrupt);	/* SIGINT handler */

    play_start(argc, argp, argv);

    cap_stop_async(0);
    
    return 0;
}
