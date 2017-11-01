#include "staff.h"

/*----------------socket初始化---------------*/
int socket_init(const char *serveraddrip,const char *serveraddrport){
	int sockfd;
	struct sockaddr_in serveraddr;
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		printf("fail to socket\n");
	}

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=inet_addr(serveraddrip);
	serveraddr.sin_port=htons(atoi(serveraddrport));

	if(connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))<0){
	    printf("fail to connect\n");
	}
	return sockfd;
}

/*-------------root用户添加函数---------------*/
void root_add(int sockfd,staff_t *add)
{
	int num=0;
    while(1)
	{
		printf("please input info  : 1 add  2 quit\n");
		scanf("%d",&num);
		getchar();
		if(1==num)
		{
			printf("please input ID\n");
			scanf("%d",&(add->log_msg.ID));  
			add->staff_msg.ID = add->log_msg.ID;
			printf("please input password\n");
			scanf("%s",(add->log_msg.password));
			printf("please input authority\n");
			scanf("%d",&(add->log_msg.authority));
			printf("please input name\n");
			scanf("%s",(add->staff_msg.name));
			printf("please input age\n");
			scanf("%d",&(add->staff_msg.age));
			printf("please input work_age\n");
			scanf("%d",&(add->staff_msg.work_age));
			printf("please input salary\n");
			scanf("%d",&(add->staff_msg.salary));
			printf("please input addr\n");
			scanf("%s",(add->staff_msg.addr));    
			printf("please input absence\n");
			scanf("%d",&(add->staff_msg.absence));
          
            add->staff_msg.mode_num=ROOT_ADD;
			send(sockfd,add,sizeof(staff_t),0);             
			if(recv(sockfd,add,sizeof(staff_t),0)==-1){
               perror("fail to recv\n");
			}
			if(0==(add->ret_flag)){
               printf("add success!\n");
			}
			else{
               perror("add fail !\n");
			}
		}
		else if(2==num)
		{
			break;
		}
	}
}

/*---------------root用户删除函数---------------*/
void root_del(int sockfd,staff_t *del)
{
	int num=0;
    while(1)
	{
		printf("please input info  : 1 del  2 quit\n");
		scanf("%d",&num);
		getchar();
		if(1==num)
		{
			printf("please input ID\n");
			scanf("%d",&(del->staff_msg.ID));  
			del->staff_msg.mode_num=ROOT_DEL;

			send(sockfd,del,sizeof(staff_t),0); 

			if(recv(sockfd,del,sizeof(staff_t),0)==-1){
				perror("fail to recv\n");
			}
			if(0==(del->ret_flag)){
				printf("del success!\n");
			}
			else{
				perror("del fail !\n");
			}
		}
		else if(2==num)
		{
			break;
		}
	}
}

/*-------------root用户查找函数---------------*/
void root_query(int sockfd,staff_t *query)
{
	int num=0;
    while(1)
	{
		printf("please input info  : 1 query  2 quit \n");
		scanf("%d",&num);
		
		getchar(); /*回收 回车 这个垃圾字符， 防止在while中打印两次printf*/

		if(1==num)
		{
			printf("please input ID\n");
			scanf("%d",&(query->staff_msg.ID));  
			query->log_msg.authority=ROOT;
            query->staff_msg.mode_num=ROOT_QUERY;

			send(sockfd,query,sizeof(staff_t),0);
			if(recv(sockfd,query,sizeof(staff_t),0)==-1){
				perror("fail to recv\n");
			}
			if(0==(query->ret_flag)){
				printf("query success!\n");
				printf("name:%s  ID:%d age:%d  work_age:%d salary:%d addr:%s absence:%d\n" ,query->staff_msg.name,\
						query->staff_msg.ID, query->staff_msg.age, query->staff_msg.work_age,\
						query->staff_msg.salary, query->staff_msg.addr, query->staff_msg.absence);
			}
			else{
			   //	perror("query fail !\n ");   //	//程序本身的错误，需要自己打印而不能用perror
			   		printf("query fail !\n");
			}
		}
		else if(2==num)
		{
			break;
		}
#if 0
		else
		{
			printf("Invalid input  \n");
			break;
		}
#endif 
	}
}

/*-------------root用户修改函数-----------------*/
void root_change(int sockfd,staff_t *change)
{
	int num=0;
	int choice=0;
    while(1)
	{
		printf("please input info  : 1 change  2 quit\n");
		scanf("%d",&num);
		getchar();
		if(1==num)
		{ 
			printf("please input ID\n");
		    scanf("%d",&(change->log_msg.ID));
			change->log_msg.authority=ROOT;
			change->staff_msg.mode_num=ROOT_CHANGE;
		   	while(1){
				printf("please make choice :1 change name  2 change password  3 quite \n");
				scanf("%d",&choice);
				if(1==choice){
				    printf("please input name\n");
				    scanf("%s",(change->staff_msg.name));

				}
			    else if(2==choice){
					printf("please input password\n");
					scanf("%s",(change->log_msg.password));
			    }
				else if(3==choice){
                    break;
				}
		    }

			send(sockfd,change,sizeof(staff_t),0); 
			if(recv(sockfd,change,sizeof(staff_t),0)==-1){
				perror("fail to recv\n");
			}
			if(0==(change->ret_flag)){
				printf("change success!\n");
			}
			else{
				perror("change fail !\n");
			}
		}
		else if(2==num)
		{
			break;
		}
	}
}

/*------------user用户显示函数-----------------*/
void show_info(int sockfd,staff_t *info)
{
	int num;
    while(1)
	{
		printf("please input info  : 1 query  2 quit\n");
		scanf("%d",&num);

		getchar();//

		if(1==num)
		{
		//	printf("please input ID\n");
		//	scanf("%d",&(info->staff_msg.ID));  
			info->log_msg.authority=USER;
			
			info->staff_msg.mode_num = ROOT_QUERY;

			send(sockfd,info,sizeof(staff_t),0);
			if(recv(sockfd,info,sizeof(staff_t),0)==-1){
				perror("fail to recv\n");
			}
			if(0==(info->ret_flag)){
				printf("info success!\n");
				printf("name:%s  ID:%d age:%d  work_age:%d salary:%d addr:%s absence:%d\n" ,\
						info->staff_msg.name,info->staff_msg.ID, info->staff_msg.age, info->staff_msg.work_age,\
						info->staff_msg.salary, info->staff_msg.addr, info->staff_msg.absence);
			}
			else{
				perror("info fail !\n");
			}
		}
		else if(2==num)
		{
			break;
		}
#if 1
		else
		{
			printf(" Invail input \n");
		}
#endif
	}
}

/*-------------user用户修改函数----------------*/
void change_password(int sockfd,staff_t *password)
{
	int num=0;
	int choice=0;
    while(1)
	{
change_password_lable:
		printf("please input info  : 1 change  2 quit\n");
		scanf("%d",&num);
		getchar();
		if(1==num)
		{ 
		//	printf("please input ID\n");
		//  scanf("%d",&(password->log_msg.ID));
			password->log_msg.authority=USER;
		   	while(1){
				printf("please make choice :1  change password  2  quite \n");
				scanf("%d",&choice);
				if(1==choice){
				    printf("please input password\n");
				    scanf("%s",(password->log_msg.password));
					password->staff_msg.mode_num = ROOT_CHANGE;
					break;
				}
				else if(2==choice){
                    goto change_password_lable; 
				}
		    }
			send(sockfd,password,sizeof(staff_t),0); 
			if(recv(sockfd,password,sizeof(staff_t),0)==-1){
				perror("fail to recv\n");
			}
			if(0==(password->ret_flag)){
				printf("password success!\n");
			}
			else{
				perror("password fail !\n");
			}
		}
		else if(2==num)
		{
			break;
		}
	}
   
}
