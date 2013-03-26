#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


int main(int argc , char **argv)
{
	char buf[5];
	int ret = 0;
	int fd;

	if(argc < 2){
		printf("Usage %s <device> <contain>\n", argv[0]);
		return -1;
	}


	fd = open(argv[1], O_RDWR);
	if(fd < 0){
		printf("open %s failed\n", argv[1]);

		return 0;
	}

#if 1
	buf[0] = 0x1;
	buf[1] = 0x3;
	buf[2] = 0x4;
	buf[3] = 0x3;
	buf[4] = 0x2;

	ret = write(fd, buf, 5);
#endif
	close(fd);

	return 0;
}
