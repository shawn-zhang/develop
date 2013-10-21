#include <stdio.h>


int main(int argc,char *argv[])
{

	//char *str = "ssid=dongle-test\nfreq=2462\nbssid=ec:17:2f:f2:bf:62\n\0";
    //char restr[20];
    unsigned int ip[4] = {0};
    unsigned char *strIp = "192.168.7.229";

/*
	if (sscanf(str,"freq=%s",restr))
	    printf("%s\n",restr);
	else
	    printf("no\n");
*/
    sscanf(strIp,"%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
    
    printf("%d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);


	return 0;
}
