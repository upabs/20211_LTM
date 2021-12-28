#ifndef __STORAGE_H__
#define __STORAGE_H__

#define ROOM_FILE "room.txt"
#define QUESTION_FILE "question.txt"
#define ACCCOUNT_STUDENT "acc_stud.txt"
#define ACCCOUNT_TEACHER "acc_teacher.txt"
#define RESULT_FOLDER "result"

typedef struct account {
    char username[100];
    char password[100];
} Account;

typedef struct Student {
    char studName[100];
    double point;
} Student;

typedef struct question {
    int id;
    int level;
    char ques[200];
    char choice[4][100];
    char answer[100];
} Question;

typedef struct room {
    char name[100];
} Room;

typedef struct room_point {
    char stud_name[100];
    char point[20];
} RoomPoint;

typedef struct node {
    void *value;
    struct node *next;
} node;
typedef struct node* Node;

typedef struct list {
    Node head;
    int count;
} List;

Room* getRoomByName(char *name);
int saveRoom(char *name);


void handleReq(char *req, char* res);
List makeQues();
void makeRes(char* res, char* op, char* mess);



Node createNode(void* value);
List newList();

void addEnd(List* l, void* value);

List getAllRooms(char *f_room);
List getAllAccount(char *acc_file);
List getAllQuestion(char *ques_file);
List getAllRoomPoint(char *file);

List getQuestionsByLevel(char *ques_file, int level);

int compareRoomByName(void* room, void* name);
int compareRoomPoint(void* rp, void* username);
int compareAccountByUserName(void* account, void* username);
int compareQuestionByLevel(void* question, void* level);

void printRoom(void* room);
void printAccount(void* account);
void printQuestion(void* quesiton);
// void printRoomPoint(void* roomPoint);

void printList(List l, void (*print)(void*));
void printRoomList(List l);
void printQuesList(List l);
void printAccList(List l);
// void printRoomPointList(List l);

Node search(List *l, void* key, int (*compare)(void*, void*));
Room* searchRoomByName(List *l, char* name);
Account* searchAccountByUsername(List *l, char* name);
RoomPoint* searchRoomPoint(List *l, char* acc_name);
Question* searchQuestionByPosition(List *l, int position);

Node deleteA(List* l, void* key, int (*compare)(void*, void*));
Room* deleteRoomByName(List *l, char* name);

int saveAllRoom(char* room_f, List l);
int saveAllQuestions(char* ques_f, List l);
int saveAllRoomPoint(char* result_f, List l);

int randUniqueArr(int arr[], int arr_size, int min, int max);
char* quesToString(Question *q, char* s);
#endif