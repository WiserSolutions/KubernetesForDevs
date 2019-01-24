#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>

#define Q_NAME    "/wiser_ipc"
#define MAX_SIZE  1024
#define M_EXIT    "done"
#define SRV_FLAG  "-producer"

int initializer();
int producer();
int consumer();

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        initializer();
    }
    else if (argc >= 2 && 0 == strncmp(argv[1], SRV_FLAG, strlen(SRV_FLAG)))
    {
        producer();
    }
    else
    {
        consumer();
    }
}

int initializer()
{
    mqd_t mq;
    struct mq_attr attr;
    char *buffer = "Shared Memory Queue - initialized";
    size_t size;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;
    
    mq = mq_open(Q_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    printf("%s\n", buffer);
    mq_send(mq, buffer, MAX_SIZE, 0);
    mq_close(mq);

    return 0;
}

int producer()
{
    mqd_t mq;
    struct mq_attr attr;
    char *buffer = NULL;
    int msg, i;
    size_t size;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(Q_NAME, O_WRONLY, 0644, &attr);
    if ((mqd_t)-1 == mq) {
        printf("Either the initializer has not been started or maybe I cannot access the same memory...\n");
        exit(1);
    }

    while (getline(&buffer, &size, stdin) != -1) {
        printf("Read: %s\n", buffer);
        fflush(stdout);
        mq_send(mq, buffer, MAX_SIZE, 0);
    }
    memset(buffer, 0, MAX_SIZE);
    sprintf(buffer, M_EXIT);
    mq_send(mq, buffer, MAX_SIZE, 0); 

    // Let the consumer startup before exiting
    sleep(3);
    mq_close(mq);
    mq_unlink(Q_NAME);
    return 0;
}

int consumer()
{
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];
    ssize_t bytes_read;
    mqd_t mq = mq_open(Q_NAME, O_RDONLY);
    if ((mqd_t)-1 == mq) {
        printf("Either the producer has not been started or maybe I cannot access the same memory...\n");
        exit(1);
    }
    do {
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
        buffer[bytes_read] = '\0';
        printf("Consumed: %s\n", buffer);
    } while (0 != strncmp(buffer, M_EXIT, strlen(M_EXIT)));

    mq_close(mq);
    return 0;
}