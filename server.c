#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "server.h"
#include "storage.h"

#define MAX 100

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections*/


int point;
char currentUserName[100];
char currentRoom[100];

int fd;

int main (int argc, char **argv)
{
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    char req[MAXLINE], res[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    //Create a socket for the soclet
    //If sockfd<0 there was an error in the creation of the socket
    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        perror("Problem in creating the socket");
        exit(2);
    }

    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    //bind the socket
    bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    //listen to the socket by creating a connection queue, then wait for clients
    listen (listenfd, LISTENQ);

    printf("%s\n","Server running...waiting for connections.");

    for ( ; ; ) {
        clilen = sizeof(cliaddr);

        //accept a connection
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
        fd = connfd;
        printf("%s\n","Received request...");

        if ( (childpid = fork ()) == 0 ) {//if it’s 0, it’s child process

            printf ("%s\n","Child created for dealing with client requests");

            //close listening socket
            close (listenfd);
            // printf("fd: %d\n", connfd);
            while (1)  {
                handle(req, res);    
            }

            if (n < 0)
                printf("%s\n", "Read error");
            exit(0);
        }
        //close socket of the server
        close(connfd);
        
    }
}

// int main() {
//     char req[MAXLINE], res[MAXLINE];
//     // strcpy(req, "LOGIN_TEACHER teacher1 111");
//     // strcpy(req, "CREATE_ROOM room4");
//     // strcpy(req, "DELETE_ROOM room4");

//     // strcpy(req, "LOGIN_TEACHER teacher1 111");
//     // strcpy(req, "SHOW_ROOM");
//     // strcpy(req, "JOIN_ROOM room3");
//     // strcpy(req, "START_TEST");
//     // strcpy(req, "ADD_QUESTION 1|Hom nay la thu may?|thu 2|thu 3|thu 4|thu 5|thu 3|");
//     handle(req, res);
// }

void handle(char *req, char *res) {
    char op[MAX], message[MAX];
    recvReq(req);
    // printf("\nreq: %s..\n", req);
    parseReq(req, op, message);

    if (strcmp(op, "LOGIN_STUDENT") == 0) {
        Account acc;
        strcpy(acc.username, strtok(message, " "));
        strcpy(acc.password, strtok(NULL, ""));
        int loginSuccess = loginStudent(acc.username, acc.password);

    } else if (strcmp(op, "LOGIN_TEACHER") == 0) {
        Account acc;
        strcpy(acc.username, strtok(message, " "));
        strcpy(acc.password, strtok(NULL, ""));
        int loginSuccess = loginTeacher(acc.username, acc.password);

    } else if (strcmp(op, "SHOW_ROOM") == 0) {
        int status = showRoom();

    } else if (strcmp(op, "CREATE_ROOM") == 0) {
        int status = createRoom(message);

    } else if (strcmp(op, "DELETE_ROOM") == 0) {
        int status = deleteRoom(message);

    } else if (strcmp(op, "JOIN_ROOM") == 0) {
        int status = joinRoom(message);

    } else if (strcmp(op, "START_TEST") == 0) {
        int status = startTest();

    } else if (strcmp(op, "ANSWER") == 0) {
        makeRes(res, "", "");
        sprintf(res, "%s %d", "ANSWER_OK", point);
        sendRes(res);
    } else if (strcmp(op, "ADD_QUESTION") == 0) {
        int status = addQuestion(message);

    } else if (strcmp(op, "SHOW_POINT") == 0) {
        int status = showPoint(message);
    }
}

int recvReq(char *req) {
    memset(req, 0, MAXLINE);
    if (0 > recv(fd, req, MAXLINE, 0)) {
        return strlen(req);
    }
    printf("\nReq: %s\n", req);
    return 0;
}

void parseReq(char *req, char *op, char *message) {
    char *next_s;
    strcpy(op, strtok(req, " "));
    if ((next_s = strtok(NULL, ""))) {
        strcpy(message, next_s);
    }
}

void makeRes(char *res, char *op, char *message) {
    memset(res, 0, MAXLINE);
    if (op != NULL && strlen(op) > 0) {
        strcat(res, op);
    }
    if (message != NULL && strlen(message) > 0) {
        if (op != NULL && strlen(op) > 0) {
            strcat(res, " ");
        }
        strcat(res, message);
    }
}

void sendRes(char *res) {
    if (res == NULL) return;
    // printf("\nRes: %s..\n", res);
    send(fd, res, strlen(res), 0);
}

int loginStudent(char *username, char *password) {
    char buf[MAXLINE] = "";
    char res[MAXLINE];

    List accStudL = getAllAccount(ACCCOUNT_STUDENT);
    Account *stud = searchAccountByUsername(&accStudL, username);
    if (stud) {
        if (strcmp(stud->password, password) == 0) {
            List roomL = getAllRooms(ROOM_FILE);
            Node n = roomL.head;
            for (int i = 0; i<roomL.count; i++) {
                Room* r = (Room*)n->value;
                if (i != 0) {
                    strcat(buf, " ");
                    strcat(buf, r->name);
                } else strcat(buf, r->name);
                n = n->next;
            }
            strcpy(currentUserName, username); // 
            makeRes(res, "LOGIN_STUDENT_OK", buf);
            sendRes(res);
            return 1;
        }
    }
    makeRes(res, "LOGIN_STUDENT_NOT_OK", "");
    sendRes(res);

    return 0;
}

int loginTeacher(char *username, char *password) {
    char res[MAXLINE];

    List accStudL = getAllAccount(ACCCOUNT_TEACHER);
    Account *acc = searchAccountByUsername(&accStudL, username);
    if (acc) {
        if (strcmp(acc->password, password) == 0) {
            makeRes(res, "LOGIN_TEACHER_OK", "");
            sendRes(res);
            return 1;
        }
    }
    makeRes(res, "LOGIN_TEACHER_NOT_OK", "");
    sendRes(res);

    return 0;
}

int createRoom(char *room_name) {
    char res[MAXLINE];
    char file[MAX] = "";
    
    List roomL = getAllRooms(ROOM_FILE);
    if (searchRoomByName(&roomL, room_name)) {
        makeRes(res, "CREATE_ROOM_NOT_OK", "");
        sendRes(res);
        return 0;
    }

    Room *r = (Room*)malloc(sizeof(Room));
    strcpy(r->name, room_name);
    addEnd(&roomL, r);
    saveAllRoom(ROOM_FILE, roomL);

    strcat(file, RESULT_FOLDER);
    strcat(file, "/");
    strcat(file, room_name);
    strcat(file, ".txt");

    // printf("file path: %s..\n", file);
    FILE *f = fopen(file, "w");
    // if (f) {fclose(f);}
    fclose(f);
    makeRes(res, "CREATE_ROOM_OK", "");
    sendRes(res);
    return 1;
}

int deleteRoom(char *room_name) {
    char res[MAXLINE];
    char file[MAX] = "";

    List roomL = getAllRooms(ROOM_FILE);
    if (deleteRoomByName(&roomL, room_name)) {
        saveAllRoom(ROOM_FILE, roomL);

        strcat(file, RESULT_FOLDER);
        strcat(file, "/");
        strcat(file, room_name);
        strcat(file, ".txt");
        
        if (remove(file) == 0) {
            // printf("deletefileok\n");
        }

        makeRes(res, "DELETE_ROOM_OK", "");
        sendRes(res);
        return 1;
    }
    makeRes(res, "DELETE_ROOM_NOT_OK", "");
    sendRes(res);
    return 0;
}

int showRoom() {
    char res[MAXLINE];
    char buf[MAXLINE] = "";

    List roomL = getAllRooms(ROOM_FILE);
    Node node = roomL.head;
    for (int i = 0; i<roomL.count; i++) {
        if (i != 0) strcat(buf, " ");
        strcat(buf, ((Room*)node->value)->name);
        node = node->next;
    }

    makeRes(res, "SHOW_ROOM_OK", buf);
    sendRes(res);
    return 1;
}

int joinRoom(char *room_name) {
    char res[MAXLINE];

    List roomL = getAllRooms(ROOM_FILE);
    if (searchRoomByName(&roomL, room_name)) {
        strcpy(currentRoom, room_name);
        makeRes(res, "JOIN_ROOM_OK", "");
        sendRes(res);
        return 1;
    }
    makeRes(res, "JOIN_ROOM_NOT_OK", "");
    sendRes(res);
    return 0;
}

int startTest() {
    char res[MAXLINE];
    char req[MAXLINE] = "";
    char buf[MAXLINE] = "";
    Question *q;
    List quesL = makeQues();
    Node n = quesL.head;
    
    point = 0;
    for (int i = 0; i<10; i++) {
        q = (Question*)n->value;
        quesToString(q, buf);

        if (i == 0)
            makeRes(res, "START_TEST_OK", buf);
        else 
            makeRes(res, "", buf);
        sendRes(res);
        recvReq(req);
        // printf("\nanswer: %s.\n", req);
        if (strcmp(req, q->answer) == 0) {
            point++;
        }
        memset(buf, 0, MAXLINE);
        n = n->next;
    }
    // printf("\npoint: %d\n", point);
    
    char f[100];
    strcpy(f, RESULT_FOLDER);
    strcat(f, "/");
    strcat(f, currentRoom);
    strcat(f, ".txt");

    List roompL = getAllRoomPoint(f);
    RoomPoint *rp = searchRoomPoint(&roompL, currentUserName);
    if (rp) {
        sprintf(rp->point, "%d", point);
    } else {
        RoomPoint *new = (RoomPoint*)malloc(sizeof(RoomPoint));
        strcpy(new->stud_name, currentUserName);
        sprintf(new->point, "%d", point);
        addEnd(&roompL, new);
    }
    saveAllRoomPoint(f, roompL);
    return 1;
}

int addQuestion(char *ques) {
    char res[MAXLINE];
    char buf[MAXLINE];

    Question *q = (Question*) malloc(sizeof(Question));
    List quesL = getAllQuestion(QUESTION_FILE);
    
    q->id = quesL.count + 1;
    q->level = atoi(strtok(ques, "|"));
    strcpy(q->ques, strtok(NULL, "|"));
    strcpy(q->choice[0], strtok(NULL, "|"));
    strcpy(q->choice[1], strtok(NULL, "|"));
    strcpy(q->choice[2], strtok(NULL, "|"));
    strcpy(q->choice[3], strtok(NULL, "|"));
    strcpy(q->answer, strtok(NULL, "|"));

    addEnd(&quesL, q);
    saveAllQuestions(QUESTION_FILE, quesL);

    makeRes(res, "ADD_QUESTION_OK", "");
    sendRes(res);
    return 1;
}

int showPoint(char *room_name) {
    char file[MAX] = "";
    char buf[MAXLINE] = "";
    char res[MAXLINE];
    
    List roomL = getAllRooms(ROOM_FILE);
    Room *r = searchRoomByName(&roomL, room_name);
    if (r) {
        strcpy(file, RESULT_FOLDER);
        strcat(file, "/");
        strcat(file, room_name);
        strcat(file, ".txt");

        List roompL = getAllRoomPoint(file);
        Node n = roompL.head;
        for (int i = 0; i<roompL.count; i++) {
            RoomPoint *rp = (RoomPoint*)n->value;
            // printf("name: %s.. point: %d..\n", rp->stud_name, rp->point);
            strcat(buf, "\n");
            strcat(buf, rp->stud_name);
            strcat(buf, "\t");
            strcat(buf, rp->point);

            printf("buf: %s..\n", buf);
            n = n->next;
        }

        makeRes(res, "SHOW_POINT_OK", buf);
        sendRes(res);
        return 1;
    } else {
        makeRes(res, "SHOW_POINT_NOT_OK", "");
        sendRes(res);
    }
    return 0;
}