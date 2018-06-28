/** 
 * @file main.c
 * @author Yimou Cao
 * @date 10/09/2017
 * Course: CSCI503
 * Lab: 3
 */
// unix socket
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include "utils.h"

#define SOCK_PATH "/tmp/ymtemp_socket"
#define X_MILLION 1


int main(int argc, char *argv[])
{
	unsigned int x_million = X_MILLION;
	int parse_status = parse_args(argc, argv, &x_million);
    if ( parse_status < 0)
    {
        exit(EXIT_FAILURE);
    }
    printf("The program would test sending %d million messages\n", x_million);
    // return 0;

	int pid;
	pid = fork();
	if (pid < 0)
	{
        printf("fork fail\n");
		exit(EXIT_FAILURE);
	}

	if ( pid == 0 )
	{
		//I am the child process and I'm sending
		sleep(1); // wait for server to set up (server listening)
		int s, len;
	    struct sockaddr_un remote;

	    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
	        perror("client: socket error");
	        exit(EXIT_FAILURE);
	    }


       	printf("Client: trying to connect...\n");

	    remote.sun_family = AF_UNIX;
	    strcpy(remote.sun_path, SOCK_PATH);
	    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
	        perror("client: connect error");
	        exit(EXIT_FAILURE);
	    }

	    printf("Client: connected.\n");

	    // set up counter
	    unsigned long sum = 0;
	    unsigned long counter = x_million * 1000000;

	    /* initialize random number generator */
	    time_t t;
	    srand((unsigned) time(&t));
	    int r;
	    // get the start time
        double start_time = get_cur_time();
        // printf("Sending starts at: %f s\n", start_time);
        // send the starting time
        if (send(s, &start_time, sizeof(start_time), 0) == -1) {
            perror("client: send error");
            exit(EXIT_FAILURE);
        }

        // send messages
        while(counter--) {
            r = rand();
            if (send(s, &r, sizeof(r), 0) == -1) {
                perror("client: send error");
                exit(EXIT_FAILURE);
            }
            sum += r;
            // printf("client: send: %d\n", r);
        }


        printf("Sender:   sum: %lu\n", sum);

		close(s);
	    
		exit(EXIT_SUCCESS);
	}
	else 
	{
		// I am the parent process and I'm receiving
        int status; // for waiting child
		int s, s2, len;
        socklen_t  t;
	    struct sockaddr_un local, remote;
	    int buff;
        double time_buff;

	    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
	        perror("server: socket error");
	        exit(1);
	    }

	    local.sun_family = AF_UNIX;
	    strcpy(local.sun_path, SOCK_PATH);
	    unlink(local.sun_path);
	    len = strlen(local.sun_path) + sizeof(local.sun_family);
	    if (bind(s, (struct sockaddr *)&local, len) == -1) {
	        perror("server: bind error");
	        exit(EXIT_FAILURE);
	    }

	    if (listen(s, 8) == -1) {
	        perror("server: listen error");
	        exit(EXIT_FAILURE);
	    }

	    
	    	int done, n;
	        unsigned long sum = 0;
	        unsigned long count = 0;

	        printf("server: waiting for a connection...\n");
	        t = sizeof(remote);
	        // accept and establish a connection
	        if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
	            perror("server: accept error");
	            exit(EXIT_FAILURE);
	        }

	        printf("server: connected.\n");

            //TODO: receive the start time
            n = recv(s2, &time_buff, sizeof(time_buff), 0);
            if (n <= 0) {
                if (n < 0) perror("server: recv timer error");
            }

            // reveicing messages
	        done = 0;
	        do {
	            n = recv(s2, &buff, sizeof(buff), 0);
	            // printf("server: received: %d\n", buff);
	            if (n <= 0) {
	                if (n < 0) perror("server: recv error");
	                done = 1;
	            }
	            else
	            {
	            	sum += buff;
	            	count++;
	            }
	        } while (!done);
            double time_diff = get_cur_time() - time_buff;
	        printf("Receiver: sum: %lu\n", sum);
            printf(">>>Sending starts at: %f s\n", time_buff);
            printf(">>>Received: %lu messages\n", count);
            printf(">>>The whole duration: %f s\n", time_diff);
            printf(">>>Throughput: %.5f msg/s\n", count/time_diff);
            //TODO: one thread receive, one thread calculate?
            //TODO: implement batch
            //TODO: calculate for each batch

	        close(s2);
	    

	    // wait child
        wait(&status);
		exit(EXIT_SUCCESS);
	}
	return 0;
}