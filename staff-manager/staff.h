#ifndef __STAFF_H__
#define __STAFF_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sqlite3.h>

#define MAX_NAME 20 		//名字最大长度
#define MAX_ADDR 50 		//地址的最大长度
#define INPUT_BUF_N 64   	//输入信息缓冲大小

#define ROOT_ADD 	1 		//添加员工信息操作码
#define ROOT_DEL 	2 		//删除员工信息操作码
#define ROOT_QUERY 	3 		//查询员工信息操作码
#define ROOT_CHANGE 4 		//修改员工信息操作码
#define QUIT        5       //员工退出服务器访问

#define ROOT 		0 		//返回时用到，超级用户权限
#define USER 		1 		//普通用户
#define FAIL 		-1 		//失败

#define SOCKADDR(arg)   	struct sockaddr##arg //## 在C/C++中具有连接字符串的作用
#define ADDRLEN(arg)   		sizeof(SOCKADDR(arg))
#define err_log(errlog)   	do{perror(errlog);exit(EXIT_FAILURE);}while(0)
#define N 			1024
#define LISNUM 		20

#define DATABASE 	"staff_manager.db" //定义数据库的名称

//员工基本信息
typedef struct{ 
	int mode_num; 			//操作类型
										//	char msgtype; 			//操作类型
	char name[MAX_NAME]; 	//姓名
	int ID; 				//工号
	int age; 				//年龄
	int work_age; 			//工龄
	int salary; 			//工资
	char addr[MAX_ADDR]; 	//地址
	int absence; 			//缺勤次数
}staff_info_t;

//登录信息
typedef struct{
	int ID; 				//用户ID
	char password[MAX_NAME];//登录密码
	int authority; 			//用户权限
}log_info_t;

typedef struct{
	int 			ret_flag;    //标识位
	staff_info_t 	staff_msg;   //员工基本信息
	log_info_t 	 	log_msg;     //登录信息
}staff_t;

extern int server_add(int confd, staff_t* msg, sqlite3* db);
extern int server_del(int confd, staff_t* msg, sqlite3* db);
extern int server_query(int confd, staff_t msg, sqlite3* db);
extern int server_change(int confd, staff_t msg, sqlite3* db);


int login(int sockfd); 						//登录界面管理
int login_process(int sockfd, staff_t *msg);//登录处理函数
int root_menu(int sockfd, staff_t *msg); 	//管理员界面
int user_menu(int sockfd, staff_t *msg); 	//用户界面
int do_quit(int sockfd, staff_t *msg); 		//退出系统处理
int input_menu();                           //界面菜单选择处理函数

extern int socket_init(const char *, const char *);
extern void root_add(int sockfd, staff_t *msg);
extern void root_del(int sockfd, staff_t *msg);
extern void root_change(int sockfd, staff_t *msg);
extern void root_query(int sockfd, staff_t *msg);
extern void show_info(int sockfd, staff_t *msg);
extern void change_password(int sockfd, staff_t *msg);

#endif
