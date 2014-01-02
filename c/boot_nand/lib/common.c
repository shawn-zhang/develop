#include <common/common.h>
#include <common/command.h>
#include <lib/ctype.h>
/*
 * NOTE! This ctype does not handle EOF like the standard C
 * library is required to.
 */
static unsigned int simple_strtoul(const char *cp)
{
	unsigned int result = 0, value, base = 0;
	
	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	
	if (!base) {
		base = 10;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	
	return result;
}

/*************************************************/
int atoi(const char *cp)
{
	if(*cp=='-')
		return -simple_strtoul(cp + 1);
	
	return simple_strtoul(cp);
}
int parse_cmd(char *line,char *argv[])
{
    int args = 0;
    
    while(*line != '\0'){
        
        argv[args++] = line;
        if (args > CFG_MAXARGS){
            args = -1;
            break;
        }
        while(*line && (*line != ' ' && (*line != '\t'))){
            line++;
        }
        if (!(*line)){
            argv[args] = NULL;
        }
        while(*line && ((*line == ' ') || *line == '\t')){
            *line = '\0';
            line++;
        }
    }
    
    return args;
}


unsigned int swapl(unsigned int d)
{
	unsigned char *p = (unsigned char *)&d;
	swap(p[0], p[3]);
	swap(p[1], p[2]);
	return d;
}

unsigned short swaps(unsigned short d)
{
	unsigned char *p = (unsigned char *)&d;
	swap(p[0], p[1]);
	return d;
}

unsigned short cal_sum(unsigned short *buf, int len)
{
	unsigned int sum = 0;

	while(len > 1)
	{
		sum += *buf;
		buf++;
		len -= 2;
	}

	if(len == 1)
		sum += *(unsigned char *)buf;

	sum = (sum >> 16) + (sum & 0xffff);

	sum += (sum >> 16);


	return (~sum) & 0xffff;
}

