#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "client.h"

#define MAXLINE 4096 /*max text line length*/
#define MAX 100
#define SERV_PORT 3000 /*port*/


int sockfd;
char res[MAXLINE], req[MAXLINE];

int main(int argc, char **argv) 
{
    // int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];
        
    //basic check of the arguments
    //additional checks can be inserted
    if (argc !=2) {
    perror("Usage: TCPClient <IP address of the server"); 
    exit(1);
    }
        
    //Create a socket for the client
    //If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        perror("Problem in creating the socket");
        exit(2);
    }
        
    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= inet_addr(argv[1]);
    servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order
        
    //Connection of the client to the socket 
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        perror("Problem in connecting to the server");
        exit(3);
    }
        
    while(1) {
        home();
    }
    exit(0);
}

// int main() {
//     home();
// }

void home() {
    int choice;
    printf("\n");
    printf("____________ HOME ____________\n");
    printf("                              \n");
    printf("   1. Login Student           \n");
    printf("   2. Login Teacher           \n");
    printf("   0. Exit                    \n");
    printf("______________________________\n\n");

    while(1) {
        printf("---> Your choice: ");
        scanf("%d", &choice);
        while(getchar() != '\n');
        
        if (choice > -1 && choice < 3)  break;
    }

    switch (choice) {
        case 1:
        case 2: 
            login(choice);
            break;
        case 0:
            printf("\n[Exit]\n");
            exit(0);
            break;
    }
}

void login(int mode) {
    char req[MAXLINE] = "";
    char buf[MAXLINE] = "";
    char username[MAX] = ""; 
    char password[MAX] = "";


    printf("\n____________Login____________\n\n");
    printf("--> username: ");
    scanf("%s", username);
    while(getchar() != '\n');

    printf("--> password: ");
    scanf("%[^\n]", password);
    while(getchar() != '\n');
    
    strcat(buf, username);
    strcat(buf, " ");
    strcat(buf, password);

    
    switch (mode) {
    case 1:
        makeReq(req, "LOGIN_STUDENT", buf);
        break;
    
    case 2:
        makeReq(req, "LOGIN_TEACHER", buf);
        break;
    }
    sendReq(req);

    // char res[MAXLINE];
    // recvRes(res);
    handleRes(res, req);
}

void startTest() {
    char buf[MAXLINE];
    int choice;
    printf("\n________________________________\n");
    printf("\n____Do you want start test?_____\n");
    printf("     1. Yes\n");
    printf("     2. No\n\n");

    while(1) {
        printf("--> Your choice: ");
        scanf("%d", &choice);
        while(getchar() != '\n');

        if (choice > 0 && choice < 3)   break;
    }

    switch (choice) {
        case 1:
            makeReq(req, "START_TEST", "");
            sendReq(req);
            break;
        case 2:
            return;
            break;
    }
    handleRes(res, req);
}

void handleRes(char *res, char *req) {
    char op[MAX], message[MAX];

    recvRes(res);
    parseRes(res, op, message);

    if (strcmp(op, "LOGIN_STUDENT_OK") == 0) {
        printf("\n[%s]\n", op);
        joinRoom(message);
        
    } else if (strcmp(op, "LOGIN_STUDENT_NOT_OK") == 0) {
        printf("\n[%s]\n", op);

    } else if (strcmp(op, "LOGIN_TEACHER_OK") == 0) {
        printf("\n[%s]\n", op);
        teacherMenu();

    } else if (strcmp(op, "LOGIN_TEACHER_NOT_OK") == 0) {
        printf("\n[%s]\n", op);

    } else if (strcmp(op, "JOIN_ROOM_OK") == 0) {
        printf("\n[%s]\n", op);

        startTest();
    } else if (strcmp(op, "JOIN_ROOM_NOT_OK") == 0) {
        printf("\n[%s]\n", op);
        
    } else if (strcmp(op, "CREATE_ROOM_OK") == 0) {
        printf("\n[%s]\n", op);
        
    } else if (strcmp(op, "CREATE_ROOM_NOT_OK") == 0) {
        printf("\n[%s]\n", op);
        
    } else if (strcmp(op, "DELETE_ROOM_OK") == 0) {
        printf("\n[%s]\n", op);
        
    } else if (strcmp(op, "DELETE_ROOM_NOT_OK") == 0) {
        printf("\n[%s]\n", op);
        
    } else if (strcmp(op, "SHOW_ROOM_OK") == 0) {
        printf("\n_________ ROOMS _________\n");
        printf("%s\n", message);
    } else if (strcmp(op, "SHOW_ROOM_NOT_OK") == 0) {
        printf("\n[%s]\n", op);
        
    } else if (strcmp(op, "SHOW_POINT_OK") == 0) {
        // printf("\n[%s]\n", op);
        printf("\n________Resutl______\n");
        printf("%s\n", message);
        
    } else if (strcmp(op, "SHOW_POINT_NOT_OK") == 0) {
        // printf("\n[%s]\n", op);
        
    } else if (strcmp(op, "START_TEST_OK") == 0) {
        printf("\n[%s]\n", op);
        answer(message);
    } else if (strcmp(op, "START_TEST_NOT_OK") == 0) {
        printf("\n[%s]\n", op);
        
    } else if (strcmp(op, "ANSWER_OK") == 0) {
        printf("\n[Score: %s]\n", message);
    } else if (strcmp(op, "ANSWER_NOT_OK") == 0) {
        printf("\n[%s]\n", op);
        
    } else if (strcmp(op, "ADD_QUESTION_OK") == 0) {
        printf("\n[%s]\n", op);
        
    } else if (strcmp(op, "ADD_QUESTION_NOT_OK") == 0) {
        printf("\n[%s]\n", op);
        
    }
}

void joinRoom(char* list_room) {
    char room_name[MAX];
    printf("\n________List room________\n");
    printf("%s\n\n", list_room);

    printf("--> Type room name: ");
    scanf("%[^\n]", room_name);
    while(getchar() != '\n');

    makeReq(req, "JOIN_ROOM", room_name);
    sendReq(req);

    handleRes(res, req);
}

void answer(char* x) {
    int i = 0;
    int c = 0;
    char question[MAXLINE], choice[4][MAX];
    do {
        if (i == 0) {
            parseQuestion(x, question, choice);
        } else {
            recvRes(res);
            parseQuestion(res, question, choice);
        }

        printf("\nCau %d: %s\n", i+1, question);
        printf("1. %s\n", choice[0]);
        printf("2. %s\n", choice[1]);
        printf("3. %s\n", choice[2]);
        printf("4. %s\n", choice[3]);
        do {
            printf("--> Your answer: ");
            scanf("%d", &c);
            while(getchar() != '\n');
        } while(c < 1 || c > 4);

        makeReq(req, "", choice[c-1]);
        sendReq(req);
        i++;
    } while (i < 10);
    makeReq(req, "ANSWER", "");
    sendReq(req);

    handleRes(res, req);
}

void parseQuestion(char *q, char *ques, char choice[][100]) {
    if (q == NULL || ques ==  NULL || choice == NULL) return;
    strcpy(ques, strtok(q, "|"));
    strcpy(choice[0], strtok(NULL, "|"));
    strcpy(choice[1], strtok(NULL, "|"));
    strcpy(choice[2], strtok(NULL, "|"));
    strcpy(choice[3], strtok(NULL, "|"));
}

void parseRes(char *res, char *op, char *message) {
    char *next_s;
    strcpy(op, strtok(res, " "));
    if ((next_s = strtok(NULL, ""))) {
        strcpy(message, next_s);
    }
}

void makeReq(char *req, char *op, char *message) {
    memset(req, 0, MAXLINE);
    if (op != NULL && strlen(op) > 0) {
        strcat(req, op);
    }
    if (message != NULL && strlen(message) > 0) {
        if (op != NULL && strlen(op) > 0) {
            strcat(req, " ");
        }
        strcat(req, message);
    }
}

void sendReq(char *req) {
    // if (req == NULL) return;
    // printf("\nReq: %s..\n", req);
    send(sockfd, req, strlen(req), 0);
}

int recvRes(char *res) {
    memset(res, 0, MAXLINE);
    if (0 > recv(sockfd, res, MAXLINE, 0)) {
        // printf("\nres: %s\n", res);
        return strlen(res);
    }
    return -1;
}


void teacherMenu() {
    int choice;
    while(1) {
        printf("\n");
        printf(" ___________ TEACHER _________ \n");
        printf("|                             |\n");
        printf("| 1. SHOW ROOM                |\n");
        printf("| 2. CREATE ROOM              |\n");
        printf("| 3. DELETE ROOM              |\n");
        printf("| 4. SHOW POINT               |\n");
        printf("| 0. EXIT                     |\n");
        printf("|_____________________________|\n");
        while(1) {
            choice = -1;
            printf("\n---> Your choice: ");
            scanf("%d", &choice);
            while(getchar() != '\n');
            
            switch (choice) {
                case 1:
                    showRoom();
                    break;
                case 2:
                    createRoom();
                    break;
                case 3:
                    deleteRoom();
                    break;
                case 4:
                    showPoint();
                    break;
                case 0:
                    printf("\n[Exit]\n");
                    break;
                default:
                    printf("\n[Try again]\n");
            }
            if (choice > -1 && choice < 5) {
                break;
            }
        }
        if (choice == 0) {
            break;
        }
    }
}

void showRoom() {
    makeReq(req, "SHOW_ROOM", "");
    sendReq(req);
    handleRes(res, req);
}

void createRoom() {
    char room_name[100]; 
    printf("\n");
    printf("__________CREATE ROOM__________\n");
    printf("---> Type room name: ");
    scanf("%[^\n]s", room_name);
    while(getchar() != '\n');

    makeReq(req, "CREATE_ROOM", room_name);
    sendReq(req);

    handleRes(res, req);
}

void deleteRoom() {
    char room_name[100]; 
    printf("\n");
    printf("_________ DELETE ROOM _________\n");
    printf("---> Type room name: ");
    scanf("%[^\n]s", room_name);
    while(getchar() != '\n');

    makeReq(req, "DELETE_ROOM", room_name);
    sendReq(req);

    handleRes(res, req);
}

void showPoint() {
    char room_name[100]; 
    printf("\n");
    printf("__________ Show point __________\n");
    printf("---> Type room name: ");
    scanf("%[^\n]s", room_name);
    while(getchar() != '\n');

    makeReq(req, "SHOW_POINT", room_name);
    sendReq(req);

    handleRes(res, req);
}