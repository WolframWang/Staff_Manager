#include "staff.h"
#define MAX_CMD 100

/*************************************************************************************
函数名  : server_add
函数功能：往数据库表1中添加员工的个人信息，往表2中添加员工的账号信息
函数参数：和客户端通信的文件描述符，数据包的首地址，数据库指针
返回值：  成功添加返回0，失败返回-1
日期：    2016-7-12
作者：    党朋程
备注：    _debug  用于调试，发回给客户端的信息:客户端判断staff结构体中的ret_flag、
		  客户端判断如果为0成功 失败是-1
************************************************************************************/
int server_add(int confd,staff_t* message,sqlite3* db)
{
    int ret = 0;
	char sql[256];  
	memset(sql,0,sizeof(sql));
	char *errmesg;
    sprintf(sql,"insert into staff_info values ('%s','%d','%d','%d','%d','%s','%d') ",\
	message->staff_msg.name,message->staff_msg.ID,message->staff_msg.age,message->\
	staff_msg.work_age,message->staff_msg.salary,message->staff_msg.addr,message->staff_msg.absence);
    if(sqlite3_exec(db,sql,NULL,NULL,&errmesg)!=SQLITE_OK)
	{
	  #ifdef _debug
      printf("%s\n",errmesg);
	  #endif
	  message->ret_flag = -1;
	  ret = -1;
    }
	else
	{
		memset(sql,0,sizeof(sql));
		sprintf(sql,"insert into log_info values ('%d','%s','%d') ",message->log_msg.ID,\
		message->log_msg.password,message->log_msg.authority);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmesg)!=SQLITE_OK)
		{
			#ifdef _debuf
			printf("%s\n",errmsg);
			#endif
			message->ret_flag = -1;
			ret = -1;
		}
		else
		{
			#ifdef _debug
			printf("insert success\n");
			#endif
			message->ret_flag = 0;
		}	 
	}
	if(send(confd,message,sizeof(staff_t),0) < 0)
	{
		#ifdef _debug
		printf("send error\n");
		#endif
		ret = -1;
	}
	return ret;
}


/*************************************************************************************
函数名  : server_del
函数功能：数据库表1中删除员工的个人信息，数据库表2中删除员工的账号信息
函数参数：和客户端通信的文件描述符，数据包的首地址，数据库指针
返回值：  成功添加返回0，失败返回-1
日期：    2016-7-12
作者：    党朋程
备注：    _debug  用于调试，发回给客户端的信息:客户端判断staff结构体中的ret_flag、
		  客户端判断如果为0成功 失败是-1
*************************************************************************************/
int server_del(int confd,staff_t* message,sqlite3* db)
{
	int ret = 0;
	char sql[256];
	memset(sql,0,sizeof(sql));
	char *errmesg;
	sprintf(sql,"delete from staff_info where id = %d ",message->staff_msg.ID);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmesg)!=SQLITE_OK)
	{
		#ifdef _debug
		printf("%s\n",errmesg);
		#endif
		message->ret_flag = -1;
		ret = -1;
	}
	else
	{
		memset(sql,0,sizeof(sql));
		sprintf(sql,"delete from log_info where id = %d ",message->log_msg.ID);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmesg)!=SQLITE_OK)
		{
			#ifdef _debug
			printf("%s\n",errmesg);
			#endif
			message->ret_flag = -1;
			ret = -1;
		}
		else
		{
			#ifdef _debug
			printf("insert success\n");
			#endif
			message->ret_flag = 0;
		}
		
	}
	if(send(confd,message,sizeof(staff_t),0) < 0)
	{
		#ifdef _debug
		printf("send error\n");
		#endif
		ret = -1;
	}
	return ret;
}


//查询回调
int callback(void *arg,int f_num,char **f_value,char **f_name)
{
	int i;
	staff_t staff;//存取查询结果
	int connectfd = *(int *)arg;

	strcpy(staff.staff_msg.name , f_value[0]);//取名字
	printf("%s\n", staff.staff_msg.name);
	staff.staff_msg.ID = atoi(f_value[1]);//取ID
	printf("%d\n",staff.staff_msg.ID);

	staff.staff_msg.age = atoi(f_value[2]);
	staff.staff_msg.work_age = atoi(f_value[3]);
	staff.staff_msg.salary = atoi(f_value[4]);
	strcpy(staff.staff_msg.addr , f_value[5]);
	staff.staff_msg.absence = atoi(f_value[6]);
	
	staff.ret_flag = 0;//成功标志,hong
	usleep(30);//防止粘包
	send(connectfd,&staff,sizeof(staff_t),0);//发送成功查询的结果

	return 0;
}
/*
 * 总查询函数
 */
int server_query(int connectfd,staff_t staff,sqlite3 *db)
{	int id;
	char cmd[MAX_CMD];//数据库指令
	char *errmsg;//数据库查询指令的反馈错误信息
	//先不判断权限了	
	id = staff.staff_msg.ID;//提取ID
	printf("id: %d\n", id);
	sprintf(cmd,"select * from staff_info where ID=%d;",id);//查询指令
#if 1
	char **result;
	int nrow;
	int ncolum;

	if(sqlite3_get_table(db, cmd, &result,&nrow, &ncolum, &errmsg) != SQLITE_OK)
	{
	
		printf("%s\n", errmsg);
		return -1;
	}
	else
	{
	
		if(nrow == 0)
		{

			staff.ret_flag = FAIL; // fail_flag 非0  FAIL=-1 
			send(connectfd,&staff,sizeof(staff_t),0);//发送Fail查询的结果
		}
		else
		{
	    	if(sqlite3_exec(db,cmd,callback,&connectfd,&errmsg) != SQLITE_OK )
			{//做查询
	    		printf("%s\n",errmsg);
	    		return -1;
			}
		} 
	}
#endif
	return 0;
}

int server_change(int connectfd,staff_t staff,sqlite3 *db)
{
	int id;//保存id;
	int authority;//保存权限
	char *errmsg;
	char cmd[MAX_CMD];
	char name[MAX_NAME];//保存要修改的名字
	char password[MAX_NAME];//保存要修改的密码

	id = staff.log_msg.ID;
	authority = staff.log_msg.authority;
	//判断权限，做不同操作
	if( authority == USER ){//user只能修改密码
		strncpy(password , staff.log_msg.password , MAX_NAME);//要修改的密码，由客户端来
		sprintf(cmd,"update log_info set password='%s' where id=%d;",password,id);
		if( sqlite3_exec(db,cmd,NULL,NULL,&errmsg) != SQLITE_OK ){
			printf("%s\n",errmsg);
			return -1;
		}

		staff.ret_flag = 0;//成功标志,hong
		

	}else if(authority == ROOT){//root修改任意值
		strncpy(name , staff.staff_msg.name , MAX_NAME);//要修改的用户名，由客户端传来
		sprintf(cmd,"update staff_info set name='%s' where id=%d;",name,id);
		if( sqlite3_exec(db,cmd,NULL,NULL,&errmsg) != SQLITE_OK ){
			printf("%s\n",errmsg);
			return -1;
		}

		staff.ret_flag = 0;//成功标志,hong
	}

	if( send(connectfd , &staff , sizeof(staff_t) , 0) < 0 ){
		perror("Fail to send change");
	}

	return 0;
}











