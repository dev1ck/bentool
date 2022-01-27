#include "protocol.h"

uint16_t cksum(void *data, uint32_t len)
{
	unsigned long sum = 0;
    uint32_t *fd;

    fd = (uint32_t*)data;

	for(; len > 1; len -=2 ) {
		sum += *fd++;

		if(sum & 0x80000000) 
			sum = (sum & 0xffff) + (sum >> 16);
	}

	if(len == 1) 
    {
		unsigned short i = 0;
		*(uint8_t *)(&i) = *(uint8_t*)fd;
		sum += i;
	}

	while(sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return (sum == 0xffff)?sum:~sum;
}