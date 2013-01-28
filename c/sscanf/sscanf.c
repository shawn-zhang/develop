#include <stdio.h>


int main(int argc,char *argv[])
{

	char *str = "ssid=dongle-test\nfreq=2462\nbssid=ec:17:2f:f2:bf:62\n\0";
        char restr[20];

	if (sscanf(str,"freq=%s",restr))
	    printf("%s\n",restr);
	else
	    printf("no\n");

	return 0;
}
