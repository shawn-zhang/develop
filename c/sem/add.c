#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define PROCNR 20

#define FNAME "/tmp/count"

#define BUFSIZE 32

static int semid;

static lock(void)
{
	static struct sembuf op = {
		.sem_num = 0,	/* GNU C extention */
		.sem_op = -1,
		.sem_flg = SEM_UNDO,
	};

	while (semop(semid, &op, 1)<0) {
		if (errno!=EINTR) {
			perror("semop(-1)");
			exit(1);
		}
	}
}

static unlock(void)
{
	static struct sembuf op = {
		.sem_num = 0,
		.sem_op = 1,
		.sem_flg = SEM_UNDO,
	};

	semop(semid, &op, 1);
}

static int count=0;

static void *thr_add(void *ptr)
{
	FILE *fp;
	char buf[BUFSIZE];
	pid_t pid;

	pid = getpid();

	fp = fopen(FNAME, "r+");
	if (fp==NULL) {
		perror("fopen()");
		exit(1);
	}

	lock();

	fgets(buf, BUFSIZE, fp);
	fseek(fp, 0, SEEK_SET);

	if (pid%3==0) {
		raise(SIGTERM);
	}

	fprintf(fp, "%ld\n", atoi(buf)+1);
	fflush(fp);

	unlock();

	fclose(fp);

	pthread_exit(NULL);
}

static void chld_handler(int s)
{
	while (waitpid(-1, NULL, WNOHANG)>0) {
		count++;
	}
}

int
main()
{
	pid_t pid;
	int i, err;
	struct sigaction sa;
	sigset_t set, unblocked;

	sa.sa_handler = chld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGCHLD, &sa, NULL);


	semid = semget(IPC_PRIVATE, 1, 0666);
	if (semid<0) {
		perror("semget()");
		exit(1);
	}

	if (semctl(semid, 0, SETVAL, 1)<0) {
		perror("semctl(..., SETVAL, ...)");
		exit(1);
	}

	for (i=0;i<PROCNR;++i) {
		pid = fork();
		if (pid<0) {
			perror("fork()");
			exit(1);
		}
		if (pid==0) {
			thr_add(NULL);
			exit(0);
		}
	}

	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);

	sigprocmask(SIG_BLOCK, &set, &unblocked);
	while (count<PROCNR) sigsuspend(&unblocked);

	sigprocmask(SIG_SETMASK, &unblocked, NULL);

	semctl(semid, 0, IPC_RMID);

	return 0;
}

