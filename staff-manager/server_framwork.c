#include "staff.h"


sqlite3 *db;
/*
 *功能:打开数据库，如果没有，则创建数据库
 *返回值: 返回数据库指针
 **/
sqlite3 *open_sqlite()
{
	sqlite3 *db;

	if(sqlite3_open(DATABASE,&db) != SQLITE_OK){
		err_log("fail to sqlite3_open");
	}

	return db;
}

/*
 *功能:创建员工信息记录表和登录信息表
 *参数:db - 数据库
 */
void create_table(sqlite3 *db)
{
	char *errmsg;
	char buf[N];

	/****************创建员工信息记录表****************/
	strcpy(buf,"create table if not exists staff_info(name char ,ID int primary key,\
		age int, work_age int, salary int, addr char, absence int);");

	if(sqlite3_exec(db,buf,NULL,NULL,&errmsg) != SQLITE_OK){
		printf("%s\n",errmsg);
	}else{
		printf("create table userinfo success.\n");

	}
	/**************************************************/

	memset(buf,0,N);

	/****************创建用户登录信息表****************/
	strcpy(buf,"create table if not exists log_info(ID int primary key, password char, authority int);");

	if(sqlite3_exec(db,buf,NULL,NULL,&errmsg) != SQLITE_OK){
		printf("%s\n",errmsg);
	}else{
		printf("create table history success.\n");
	}
	/**************************************************/

	return;
}

int sock_init(const char *argv[])
{
	int sockfd;
	SOCKADDR(_in) serveraddr;

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
		err_log("fail to socket");
	}
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));

	if(bind(sockfd,(SOCKADDR()*)&serveraddr,ADDRLEN()) < 0){
		err_log("fail to bind");
	}

	if(listen(sockfd,LISNUM) < 0){
		err_log("fail to listen");
	}

	return sockfd;
}

int accept_connect(int sockfd, SOCKADDR(_in)*clientaddr, socklen_t *clientlen)
{
	int confd;

	if((confd = accept(sockfd,(SOCKADDR()*)clientaddr,clientlen)) < 0){
		err_log("fail to accept");
	}

	return confd;
}


int search_user(sqlite3 *db,char *cmd,int confd, staff_t *msg)
{
	//char buf[N];
	char **result;
	char *errmsg;
	int nrow;
	int ncolum;

	if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolum,&errmsg) != SQLITE_OK){
		printf("%s\n",errmsg);
		return -1;
	}else{

		if(nrow == 0){
			msg->log_msg.authority = -1;
			send(confd,msg,sizeof(staff_t),0);
			return -1;

		}else{
			msg->log_msg.authority = atoi(result[5]);
			send(confd,msg,sizeof(staff_t),0);
			return 1;
		}
	}

	return 0;
}

/*
 *功能：匹配数据库登录信息并处理
 *
 *
 */
int do_login(sqlite3 *db, staff_t *msg, int confd)
{
	int ret;
	char res[128];
	log_info_t log_msg = msg->log_msg;
	printf("%d,%s\n",msg->log_msg.ID,msg->log_msg.password);

	/**************将客户端的姓名和密码拼接成sql插入语句************/
	sprintf(res,"select * from log_info where ID=%d and password='%s';",log_msg.ID,log_msg.password);
	printf("%s\n",res);

	/**************判断用户输入的姓名和密码是否正确*****************/
	if(search_user(db,res,confd,msg) < 0){
		return -1;
	}
		
	return 0;
}

/*
 *功能：线程处理函数
 *参数：套接字connectfd
 *
 */
void *thread_handle(void *arg)
{
	int ret;
	int confd = *(int *)arg;

	staff_t msg;
	while(1){
		ret  =	recv(confd,&msg,sizeof(staff_t),0);
		if( ret == 0){
			printf("未收到登录信息\n");
			pthread_exit(NULL);
		}else if (ret < 0){
			printf("Fail to recv\n");
		}

		if (ret < 0){
			perror("162line recv < 0");;
		}

		if(msg.staff_msg.mode_num == QUIT){
			pthread_exit(NULL);
		}
		if(do_login(db,&msg,confd) == 0){
			break;
		}
	}
	
	while(1)
	{
		ret = recv(confd,&msg,sizeof(msg),0);

		if( ret == 0){
			printf("未收到信息\n");
			pthread_exit(NULL);
		}else if (ret < 0){
			printf("Fail to recv\n");
		}

		switch(msg.staff_msg.mode_num){
			case ROOT_ADD:
				server_add(confd,&msg,db);
				break;				
			case ROOT_DEL:
				server_del(confd,&msg,db);
				break;
			case ROOT_QUERY:
				server_query(confd,msg,db);
				break;
			case ROOT_CHANGE:
				server_change(confd,msg,db);
				break;
			
		}

	}
}

int main(int argc, const char *argv[])
{
	
	int sockfd;
	char buf[N];
	int connectfd;
	pthread_t thread_id;

	if(argc < 3){
		argv[1] = "190.5.0.62";
		argv[2] = "9999";
	}

	SOCKADDR(_in) clientaddr;
	socklen_t clientlen = ADDRLEN();

	sockfd = sock_init(argv);

	db = open_sqlite();
	create_table(db);
	
	while(1){
		connectfd = accept_connect(sockfd,&clientaddr,&clientlen);
		
		if(pthread_create(&thread_id,NULL,thread_handle,&connectfd) != 0){
			err_log("fail to pthread");
		}

		pthread_detach(thread_id);
	}

	return 0;
}
