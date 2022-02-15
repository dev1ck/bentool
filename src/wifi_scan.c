#include "wifi.h"

int g_wifi_scan_end_signal = 0;

void INTwifiScanHandler(int sig)
{
    signal(sig, SIG_IGN);
    g_wifi_scan_end_signal = 1;
}

int wifi_scan(char* ifname, char* input_chan)
{
	int n, ret, chan, chan_fixd = 0;
	uint32_t chans[8]={0,};
    struct iw_dev dev;
    struct ap_info api;
	struct ap_list apl;
	struct timeval s_tv, n_tv, p_tv, p_tv2;
	suseconds_t msec, p_msec;

	iw_init_dev(&dev);
	strncpy(dev.ifname, ifname, sizeof(dev.ifname)-1);
    if(iw_dev_set(&dev)<0)
        return -1;

	if (input_chan == NULL)
	{
		for (n = 1; n<=13; n++)
			channel_set(chans,n);
		for (n = 36; n<=64; n+=4)
			channel_set(chans,n);
		for (n = 100; n<=140; n+=4)
			channel_set(chans,n);
		for (n = 149; n<=165; n+=4)
			channel_set(chans,n);

		chan = n = 0;
		do 
		{
			chan = (chan % CHANNEL_MAX) + 1;
			if (channel_isset(chans, chan))
				ret = iw_set_channel(&dev, chan);
			else
				ret = -1;
			/* 실패시 다음 채널 탐색 */
		} while(++n < CHANNEL_MAX && ret < 0);
		if (ret < 0)
			return -1;
	}
	else
	{
		chan = atoi(input_chan);
		if(chan <= 0 || chan > CHANNEL_MAX)
		{
			printf("Channel insert error\n");
			close(dev.fd_in);
			close(dev.fd_out);
			return -1;
		}
		if(iw_set_channel(&dev, chan)<0)
			return -1;

		chan_fixd = 1;
	}

	memset(&api, 0, sizeof(api));
	memset(&apl, 0, sizeof(apl));

	signal(SIGINT, INTwifiScanHandler);

    clear_scr();
    update_scr(&apl, &dev);

	gettimeofday(&s_tv, NULL);
    gettimeofday(&p_tv, NULL);

	while(!g_wifi_scan_end_signal)
	{		
		if(!get_ap_info(&dev, &api))
        {
            if(input_chan == NULL)
                add_ap(&apl, &api);
            else
            {
                if(api.chan == dev.chan)
                    add_ap(&apl, &api);
            }    
        }
			
        gettimeofday(&p_tv2, NULL);
		if (p_tv2.tv_usec > p_tv.tv_usec)
			p_msec = p_tv2.tv_usec - p_tv.tv_usec;
		else
			p_msec = p_tv.tv_usec - p_tv2.tv_usec;

        if (p_msec >= 500000) {
			update_scr(&apl, &dev);
			gettimeofday(&p_tv, NULL);
		}

		gettimeofday(&n_tv, NULL);
		if(n_tv.tv_usec > s_tv.tv_usec)
			msec = n_tv.tv_usec - s_tv.tv_usec;
		else
			msec = s_tv.tv_usec - n_tv.tv_usec;
		
		if (msec >= 100000 && !chan_fixd)
		{
			n = 0;
			do {
				chan = (chan % CHANNEL_MAX) + 1;
				if (channel_isset(chans, chan))
					ret = iw_set_channel(&dev, chan);
				else
					ret = -1;
			} while(++n < CHANNEL_MAX && ret < 0);
			if (ret < 0) 
				return -1;
			gettimeofday(&s_tv, NULL);
		}
	}
	
	printf("Exiting...\n");
	iw_dev_unset(&dev);
	free_ap_list(&apl);

    return 0;
}