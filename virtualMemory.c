#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

pid_t MMU_pid;


// PAGE TABLE ENTRY
typedef struct {
    int Valid;
    int Frame;
    int Dirty;
    int Requested;
} page_table_entry;

typedef page_table_entry* page_table_pointer;

void sig_handler(int sig) {
	for (i = 0; i < num_pages; i++) {
		if (page_table[i]->Requested) {

		}
	}
	kill(MMU_pid, SIGCONT);
}

int main(int argc, char * argv[])
{
	int i;

	// memory access
	char *mem_accesses[];


	// fork and wait vars
	int status;
	pid_t pid;

	// page table
	int num_pages = argv[0];
	int page_table_size = num_pages * sizeof(page_table_entry);
	page_table_pointer page_table;

	// shared memory
	int shmid = shmget(IPC_PRIVATE, page_table_size, 0777 | IPC_CREAT);
	
	pid = fork();
	if (pid == -1) {
		perror("Fork failed");
		exit(1);
	} else if (pid == 0) {
		char *argv_list[] = {num_pages, mem_accesses, getppid()};
		execv("mmu.o", argv_list);
	} else {
		MMU_pid = pid;
		page_table = (page_table_pointer) shmat(shmid, 0, 0);

		// init page table
		for (i = 0; i < num_pages; i++) {
			page_table[i]->Valid = 0;
		}

		// wait for SIGUSR1
		while (1) {
			signal(SIGUSR1, sig_handler);
		}

		waitpid(MMU_pid, &status, 0); // wait for MMU
		shmdt(page_table);  // detach from page table
		shmctl(shmid, IPC_RMID, 0);  // destroy page table

		// printf("In Parent.\tMy ID: %d\n", getpid());
		// printf("Wait is succesful with status %d\n", status);
	}

	return 0;
}