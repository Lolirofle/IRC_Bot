#include "pipes.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

struct PipedStream p2open(char* path,char* const argv[]){
	/**
	 * Visualization of the process:
	 *
	 *       |--------|
	 * Input |  Pipe  | Output
	 *  end  |--------|  end
	 *          <-->
	 * When no end is closed, data can flow
	 * in both directions. E.g. if the out-
	 * put end is closed, data can only
	 * enter through the input end and flow
	 * out from the output end from the 
	 * parent process' perspective.
	 * ====================================
	 *             ___________
	 * |---------||           ||---------|
	 * | inPipe  ||  Program  || outPipe |
	 * |---------||___________||---------|
	 *     -->     Process text    -->
	 * inPipe has a closed output end and
	 * outPipe has a closed input end. This
	 * makes it possible to read from the 
	 * outPipe and write to the inPipe.
	 */

	pid_t pid;
	int inPipe[2];
	int outPipe[2];

	//Create pipes
	if(pipe(inPipe)!=0 || pipe(outPipe)!=0){
		fprintf(stderr,"Creation of pipes failed\n");
		return (struct PipedStream){NULL,NULL,0};
	}

	//Create child process
	pid=fork();

	switch(pid){
		//Forking failed
		case -1:
			close(inPipe[0]);
			close(inPipe[1]);
			close(outPipe[1]);
			close(outPipe[1]);
			fprintf(stderr,"Fork failed\n");
			return (struct PipedStream){NULL,NULL,0};

		//Child process (Subprocess)
		case 0:
			//Close other ends
			close(inPipe[1]);
			close(outPipe[0]);

			dup2(inPipe[0],STDIN_FILENO);
			dup2(outPipe[1],STDOUT_FILENO);
			close(inPipe[0]);
			close(outPipe[1]);

			//Execute program
			execv(path,argv);

			//This will be reached if error
			_exit(1);

		//Parent process (Current process)
		default:
			//Close input pipe reading end. Able to write to the input end
			close(inPipe[0]);
			//Close output pipe writing end. Able to read from the output end
			close(outPipe[1]);

			if(inPipe[1]>=0 && outPipe[0]>=0){
				FILE* in,
					* out;
				if(!(in=fdopen(inPipe[1],"wb")))
					close(inPipe[1]);
				if(!(out=fdopen(outPipe[0],"rb")))
					close(outPipe[0]);

				return (struct PipedStream){
					in,
					out,
					pid
				};
			}
	}
	return (struct PipedStream){NULL,NULL,0};
}

int p2close(struct PipedStream stream){
    int status;
	pid_t pid;

	//Wait for child process. If it's interrupted, try again
	do{
		pid=waitpid(stream.pid,&status,0);
	}while(pid==-1 && errno==EINTR);

	//If the process ended normally
	if(WIFEXITED(status)){
		//Return its exit code
		return WEXITSTATUS(status);
	}

	//Return -1 when it hasn't ended normally
    return -1;
}

extern void p2flushWrite(struct PipedStream stream);
extern void p2flushRead(struct PipedStream stream);

/*
//Testing
int main(void){
	char* argv[]={"morse",NULL};
	struct PipedStream stream=p2open("../../bin/external_commands/yt",argv);

	char input[] = "åäö";
	char output[512];

	//Write
	fputs(input,stream.in);
	p2flushWrite(stream);

	printf("Sent: %s\n",input);

	//Read
	//fputs("Received: \"",stdout);
	//int c;while((c=fgetc(stream.out))!=EOF)
	//	putchar(c);
	//putchar('"');
	//putchar('\n');
	
	size_t len=fread(output,sizeof(char),512,stream.out);
	printf("Received: %.*s\n",(int)len,output);

	p2flushRead(stream);

	printf("Exit code: %i\n",p2close(stream));

	return 0;
}
*/
