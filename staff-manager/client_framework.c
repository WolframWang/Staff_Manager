
#include "staff.h"


int main(int argc, const char *argv[])
{
	int sockfd;

	if(argc != 3){
		fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sockfd = socket_init(argv[1], argv[2]); 		
	login(sockfd);

	return 0;
}


int login(int sfd)
{
	int choice;
	int sockfd;
	staff_t msg;

	sockfd = sfd;

	while(1){

		printf("****************************************\n"); 
		printf("******      员工信息管理系统      ******\n");
		printf("****************************************\n");
		printf("**     1:登录          2:退出系统     **\n");
		printf("****************************************\n");
		
		printf("line 38 socked=%d\n",sockfd);	

login_label:
		printf("please choose : ");
		choice = input_menu();
		if(choice <= 0 || choice > 2){
			if(choice < 0 || choice > 2){
				printf("Invalid input.\n");
			}
			goto login_label;
		}
		else{
			switch(choice){
			case 1:
				login_process(sockfd, &msg);
				break;
			case 2:
				do_quit(sockfd, &msg);
				exit(0);
			}
		}
	}

	return 0;
}


int login_process(int sockfd, staff_t *msg)
{
	char buf[INPUT_BUF_N] = {0};

	// 输入登录信息
login_process_label:

	printf("请输入用户ID：");
	fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf) - 1] = '\0';
	msg->log_msg.ID = atoi(buf);
	memset(buf, 0, strlen(buf));

	if(msg->log_msg.ID <= 0) {
		printf("输入有误，请重新输入!\n");
		goto login_process_label;
	}

	printf("请输入密  码：");
	fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf) - 1] = '\0';
	strncpy(msg->log_msg.password, buf, sizeof(buf));
	memset(buf, 0, strlen(buf));

	//填充消息结构体，发送登录信息
	//	msg->log_msg.authority = FAIL;
	printf("line 85 socked=%d\n",sockfd);
	if( send(sockfd, msg, sizeof(staff_t), 0) < 0 ){
		printf("Fail to send login");
		goto login_process_label;
	}

	//接收服务器返回的结果
	if( recv(sockfd, msg, sizeof(staff_t), 0) < 0 ){
		err_log("Fail to recv login back");
	}

	//判断
	printf("----%d--------\n", msg->log_msg.authority);
	int root = msg->log_msg.authority;
	switch(root) {
	case 0:
		root_menu(sockfd, msg);
		break;
	case 1:
		msg->staff_msg.ID = msg->log_msg.ID;
		user_menu(sockfd, msg);
		break;
	case -1:
		printf("用户名或密码错误！--- root:%d\n", root);
		break;
	default:
		printf("---err_log------\n");
	}

	return 0;
}


int root_menu(int sockfd, staff_t *msg)
{
	int choice;
	
	int ID = NULL;
	ID = msg->log_msg.ID;

	while(1){
		printf("****************************************\n");
		printf("**       员工信息管理系统  %-8d   **\n", ID);
		printf("****************************************\n");
		printf("**  1:增加       2:删除       3:查找  **\n");
		printf("**  4:修改       5:退出系统           **\n");
		printf("****************************************\n");

root_label:
		printf("please choose : ");
		choice = input_menu();
		if(choice <= 0 || choice > 5){
			if(choice < 0 || choice > 5){
				printf("Invalid input.\n");
			}
			goto root_label;
		}
		else{
			switch(choice){
			case 1:
				root_add(sockfd, msg);
				break;
			case 2:
				root_del(sockfd, msg);
				break;
			case 3:
				root_query(sockfd, msg);
				break;
			case 4:
				root_change(sockfd, msg);
				break;
			case 5:
				do_quit(sockfd, msg);
				exit(0);
			}
		}
	}

	return 0;
}


int user_menu(int sockfd, staff_t *msg)
{	
	int choice;
	int ID = msg->log_msg.ID;

	while(1){
		printf("****************************************\n");
		printf("**       员工信息管理系统  %-8d  **\n", ID);
		printf("****************************************\n");
		printf("** 1:查询信息  2:修改密码  3:退出系统 **\n");
		printf("****************************************\n");

user_label:
		printf("please choose : ");
		choice = input_menu();
		if(choice <= 0 || choice > 3){
			if(choice < 0 || choice > 3){
				printf("Invalid input.\n");
			}
			goto user_label;
		}
		else{
			switch(choice){
			case 1:
				show_info(sockfd, msg);
				break;
			case 2:
				change_password(sockfd, msg);
				break;
			case 3:
				do_quit(sockfd, msg);
				exit(0);
			}
		}
	}

	return 0;
}


int do_quit(int sockfd, staff_t *msg)
{
	msg->staff_msg.mode_num = QUIT;
	if( send(sockfd, msg, sizeof(staff_t), 0) < 0 ){
		err_log("Fail to send");
	}

	close(sockfd);	

	printf("Success quit !!\n");
	return 0;
}


int input_menu()
{
	/*Success, return choice(1-9);  no input, return 0;  input error, return -1; */

	int choice;
	char buf[INPUT_BUF_N] = {0};

	fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf) - 1] = '\0';
	if(strlen(buf) == 0){
		return 0;
	}
	else if(strlen(buf) == 1){
		if((buf[0] >= '1') && (buf[0] <= '9')){
			choice = atoi(&buf[0]);
			return choice;
		}
	}
	else
	  	return -1;
}
