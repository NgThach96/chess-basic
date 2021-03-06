#include "decode.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h> /* for strncpy */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
//bien cuc bo for socket

#define PORT 3000
#define BUF_SIZE 2000
#define CLADDR_LEN 100

// Thạch, define server's address
#define BIG_SERVER "127.0.0.1" 
#define HOST_PORT 3001

int sockfd, ret;
int sockfd, len, ret, newsockfd;
struct sockaddr_in addr, cl_addr;
char code[BUF_SIZE];
pid_t childpid;
char clientAddr[CLADDR_LEN];
char *serverAddr;

int clicks = 0;
int player = 0;
int player_color;
int isServer;

char note[5];
char mnum[11]; // max int size is 10 chars long + 0 char
int move[4];   //src row col; dest row col
int board[8][8];
int movecnt = 0;

//thread
GThread *thread;
GError *error = 0;
static void updateGUI();
extern int makemove(int player, int *move, int board[][8]);
extern decoded getresults(char *input);

// Thạch, get current computer's ip
static char* getCurrentIP() {
    char *address;
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "wlp3s0", IFNAMSIZ - 1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    // result
    address = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    return address;
}

// Thạch, send 1 message to server
static int send1MessageToBigServer(char *message)
{
    char *serverAddr;
    char temp[30];
    struct sockaddr_in addr;
    int sockfd, ret, n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("error creating socket\n");
        exit(1);
    }
    serverAddr = BIG_SERVER;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverAddr);
    addr.sin_port = PORT;
    ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        printf("Error connecting to server!\n");
        exit(1);
    }
    ret = send(sockfd, message, strlen(message) + 1, 0);
    if (ret < 0)
    {
        printf("Eror when send message \n");
        return -1;
    }
    return sockfd;
}

static void play(GtkWidget *widget, GtkWidget *window)
{
    g_print("Play!\n");
    gtk_widget_show_all(window);
    gtk_widget_hide(gtk_widget_get_toplevel(widget));
}

static void receiveCmd(void *socket)
{
    int sockfd, ret;
    char code_rcv[BUF_SIZE];
    sockfd = (int)socket;
    memset(code_rcv, 0, BUF_SIZE);
    for (;;)
    {
        ret = recvfrom(sockfd, code_rcv, BUF_SIZE, 0, NULL, NULL);
        if (ret > 0)
        {
            gdk_threads_enter();
            decoded temp1;
            temp1 = getresults(code_rcv);
            switch (temp1.cmd)
            {
            case 1:
                printf("1\n");
                move[0] = temp1.var[0] - '0';
                move[1] = temp1.var[1] - '0';
                move[2] = temp1.var[2] - '0';
                move[3] = temp1.var[3] - '0';
                makemove(player, move, board);
                printf("2\n");
                updateGUI();
                break;
            case 4:
                // Thạch
                player_color = temp1.var[0] - '0';
                break;
            default:
                break;
            }
            gdk_threads_leave();
        }
    }
}

static void make_host(GtkWindow *parent, gchar *input)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("error creating socket\n");
        exit(1);
    }
    puts(input);
    serverAddr = input;
    puts(serverAddr);
    printf("socket created\n");
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverAddr);
    addr.sin_port = HOST_PORT;
    ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        printf("Error connecting to server!\n");
        exit(1);
    }

    printf("connected to server\n");
    memset(code, 0, BUF_SIZE);
    thread = g_thread_create(receiveCmd, (void *)sockfd, FALSE, &error);
    if (!thread)
    {
        g_print("Error: %s\n", error->message);
        return (-1);
    }
    gtk_widget_destroy(parent);
}

static void make_client(GtkWindow *parent, gchar *input)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("error creating socket\n");
        exit(1);
    }
    puts(input);
    serverAddr = BIG_SERVER;
    puts(serverAddr);
    printf("socket created\n");
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverAddr);
    addr.sin_port = PORT;

    ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        printf("Error connecting to server!\n");
        exit(1);
    }
    printf("connected to server\n");
    char message[50];
    strcpy(message, "CNTS   ");
    ret = send(sockfd, message, strlen(message) + 1, 0);
    if (ret < 0)
    {
        printf("Eror when send message \n");
        return -1;
    }
    memset(code, 0, BUF_SIZE);
    thread = g_thread_create(receiveCmd, (void *)sockfd, FALSE, &error);
    if (!thread)
    {
        g_print("Error: %s\n", error->message);
        return (-1);
    }
    gtk_widget_destroy(parent);
}

static void make_server()
{
    // Thạch, Get current IP end send to big server
    char temp[30];
    char* currentIp = getCurrentIP();
    strcpy(temp, "HOST  ");
    strcpy(temp + strlen(temp), currentIp);
    printf("%s\n",temp);
    int i = send1MessageToBigServer(temp);
    close(i);
    if(i < 0) {
        printf("Error send message to big Server\n");
     }
    // khoi tao server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Error creating socket!\n");
        exit(1);
    }
    printf("Socket created...\n");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = HOST_PORT;

    ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        printf("Error binding!\n");
        exit(1);
    }
    printf("Binding done...\n");
    printf("Waiting for a connection...\n");
    listen(sockfd, 5);
    len = sizeof(cl_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cl_addr, &len);
    if (newsockfd < 0)
    {
        printf("Error accepting connection!\n");
        exit(1);
    }

    inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, CLADDR_LEN);
    printf("Connection accepted from %s...\n", clientAddr);

    memset(code, 0, BUF_SIZE);

    thread = g_thread_create(receiveCmd, (void *)newsockfd, FALSE, &error);
    if (!thread)
    {
        g_print("Error: %s\n", error->message);
        return (-1);
    }
}

static void control(char *input)
{
}