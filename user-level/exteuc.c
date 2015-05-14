#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

#define READ_BUF 4096

void err_sys(char *err) {
	perror(err);
	exit(1);
}

void e_euclid(int e, int phi1 , int phi2, int phi3) {
	pid_t pid;
	int fd[2];

	if(pipe(fd) < 0) {
		err_sys("pipe error");
	}

	if((pid = fork()) < 0) {
		err_sys("fork error");
	} 
	else if(pid > 0) { // parent
		close(fd[1]); // close write end

		if(fd[0] != STDIN_FILENO) { // set STDIN
			if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO) {
				err_sys("dup2 error");
			}
		}

		char buf[READ_BUF];
		if(read(STDIN_FILENO, buf, READ_BUF) < 0) {
			err_sys("read error");
		}

		printf("[received]: %s\n", buf);

		if (waitpid(pid, NULL, 0) < 0)
            err_sys("waitpid error");
        
        exit(0);
	} 
	else { // child
		close(fd[0]); // close read end

		if(fd[1] != STDOUT_FILENO) { // set STDOUT
			if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO) {
				err_sys("dup2 error");
			}
		}

		char e_s[READ_BUF]; 

		char phi_s[READ_BUF];
		char phi2_s[READ_BUF]; 
		char phi3_s[READ_BUF];
		
		snprintf(e_s, READ_BUF, "%d\n", e);

		snprintf(phi_s, READ_BUF, "%d\n", phi1);
		snprintf(phi2_s, READ_BUF, "%d\n", phi2);
		snprintf(phi3_s, READ_BUF, "%d\n", phi3);

		// execute Python script
		execlp("python", "python", "exteuc.py", e_s, phi_s, phi2_s, phi3_s, (char *)NULL);
	}
}

int main() {

	e_euclid(2, 0, 2, 5);
	return 0;
}