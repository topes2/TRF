#ifndef CONFIGS_H
#define CONFIGS_H

//includes for the code


//server
#define PORT 5567
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

//Q&A
#define MAX_ANSWER 100
#define MAX_LEGTH 1024
#define ASK_CODE "&@%$@"
#define ANSWER_CODE "@$^@^"
#define LISTQUESTIONS_CODE "$^&%@"

//files
#define PUTFILES_CODE "#!%#!"
#define LISTFILES_CODE "^%!&#"
#define GETFILES_CODE "$*&!@"

//users
#define MAX_USERNAME_LENGTH 10
#define MAX_PASSWORD_LENGTH 51
#define LOGIN_CODE "(!+{';!$^)"
#define STOP_LOOP "#$@#$@"
#define WELCOME_MSG "Welcome\nEnter the command: IAM username WITHPASS yourpassword\n"
#define SERVER_BY_NAME "localhost"

//Attendance
#define LATE_TIME 1200
#define ABSENT_TIME 2700

#endif 