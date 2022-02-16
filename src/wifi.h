#ifndef _WIFI_H
#define _WIFI_H

#pragma once

#include "protocol.h"
#include <linux/wireless.h>
#include <malloc.h>

#define ESSID_LEN 32

#define FRAME_CONTROL_TYPE_MGMT_FRAME 0
#define FRAME_CONTROL_SUBTYPE_DEAUTH 12
#define FRAME_CONTROL_SUBTYPE_BEACON 8

#define INFO_ELEMENT_ID_SSID 0
#define INFO_ELEMENT_ID_DS 3

#define CHANNEL_MAX 255

#define RADIOTAP_F_PRESENT_RATE	(1<<2)
#define RADIOTAP_F_PRESENT_TX_FLAGS	(1<<15)
#define RADIOTAP_F_TX_FLAGS_NOACK	0x0008
#define RADIOTAP_F_TX_FLAGS_NOSEQ	0x0010

struct ap_info {
	int chan;
	uint8_t bssid[IFHWADDRLEN];
	uint8_t essid[ESSID_LEN+1];
};

struct radiotap_hdr {
	uint8_t  version;
	uint8_t  pad;
	uint16_t len;
	uint32_t present;
} __attribute__((__packed__));

struct write_radiotap_data {
	uint8_t  rate;
	uint8_t  pad;
	uint16_t tx_flags;
} __attribute__((__packed__));

struct frame_control {
	uint8_t protocol_version:2;
	uint8_t type:2;
	uint8_t subtype:4;
	uint8_t to_ds:1;
	uint8_t from_ds:1;
	uint8_t more_frag:1;
	uint8_t retry:1;
	uint8_t pwr_mgt:1;
	uint8_t more_data:1;
	uint8_t protected_frame:1;
	uint8_t order:1;
} __attribute__((__packed__));

struct sequence_control {
	uint16_t fragment:4;
	uint16_t sequence:12;
} __attribute__((__packed__));


struct mgmt_frame {
	struct frame_control fc;
	uint16_t duration;
	uint8_t  dest_mac[IFHWADDRLEN];
	uint8_t  src_mac[IFHWADDRLEN];
	uint8_t  bssid[IFHWADDRLEN];
	struct sequence_control sc;
	uint8_t  frame_body[];
} __attribute__((__packed__));

struct beacon_frame_body
{
	uint64_t timestamp;
	uint16_t interval;
	uint16_t capabilities;
	uint8_t  infos[];
} __attribute__((__packed__));

struct info_element
{
	uint8_t id;
	uint8_t len;
	uint8_t info[];
} __attribute__((__packed__));

struct deauth_packet
{
	struct radiotap_hdr rt_hdr;
	struct write_radiotap_data w_rt_data;
	struct mgmt_frame deauth;
	
} __attribute__((__packed__));

struct iw_dev
{
	char ifname[IFNAMSIZ+1];
	int ifindex;
	int fd_in;
	int fd_out;
	volatile int chan;
	struct ifreq old_flags;
	struct iwreq old_mode;
};

struct access_point
{
	volatile unsigned int num_of_deauths;
	time_t last_beacon_tm;
	uint16_t sequence:12;
	struct ap_info info;
	struct access_point *next;
	struct access_point *prev;
};

struct ap_list
{
	struct access_point *head;
	struct access_point *tail;
};

struct deauth_thread_args
{
	struct ap_list *apl;
	struct iw_dev *dev;
	char *SSID;
	int ap_find_stop;
	pthread_mutex_t *list_mutex;
	pthread_mutex_t *ap_find_stop_mutex;
};

void iw_init_dev(struct iw_dev *dev);
int iw_dev_set(struct iw_dev *dev);
void iw_dev_unset(struct iw_dev *dev);
int iw_set_channel(struct iw_dev *dev, int chan);

void make_deauth_packet(struct deauth_packet **packet, struct iw_dev *dev, struct access_point *ap);
int send_deauth(struct iw_dev *dev, struct access_point *ap);

void channel_set(uint32_t *cs, uint8_t chan);
int channel_isset(uint32_t *cs, uint8_t chan);

int add_ap(struct ap_list *apl, struct ap_info *api);
int get_ap_info(struct iw_dev *dev, struct ap_info *api);
void free_ap_list(struct ap_list *apl);

void clear_scr();
void scan_update_scr(struct ap_list *apl, struct iw_dev *dev);
void attack_update_scr(struct ap_list *apl, struct iw_dev *dev);


#endif