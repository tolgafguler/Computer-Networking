#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#define MAX 100

//Struct for clients
struct Clients {
   char  Id[MAX];
   char  Ip_adress[MAX];
   char  Mac_adress[MAX]; 
   char  routing_table[MAX][MAX];
   struct Queue* in;
   struct Queue* out;
};
//Struct for layers 
struct Layer {
	char** info;
	struct Layer* next;
};
//Struct for stacks, top holds the top of the every stack
struct Stack{
	struct Layer* top;
	struct Stack* next; 
};
//Struct for Queues 
struct Queue{
	struct Stack* front;
	struct Stack* rear;
};

//This function pushes each layer to stack
struct Stack* push(struct Stack *stack,struct Layer *layer,int info_size){
	int i,j,k;
	struct Layer* ptr;
	ptr=(struct Layer*)malloc(sizeof(struct Layer));
	ptr->info = (char**) malloc (info_size*sizeof(char*));
	for(k = 0; k<info_size; k++)
	{			
		ptr->info[k] = (char*)malloc(MAX*sizeof(char));
	}
	for(i=0;i<info_size;i++)
	{
		strcpy(ptr->info[i],layer->info[i]);
	}	
	ptr->next=stack->top;
	stack->top=ptr;
	return stack;
}

//This funtion pop the top of the stack
struct 	Layer* pop(struct Stack *stack,struct Layer *layer){
	
	struct Layer* ptr=stack->top;
	stack->top=stack->top->next;
	return ptr;
}

//This function remove a stack from rear of queue  
void enqueue(struct Stack* s,struct Queue* q){
	struct Stack* temp=(struct Stack*)malloc(sizeof(struct Stack));
	temp->top=s->top;
	temp->next=NULL;
	if(q->front==NULL && q->rear==NULL)
	{
		q->front=q->rear=temp;
		return;
	}
	q->rear->next=temp;
	q->rear=temp;
}

//This function add stack to front od queue 
void dequeue(struct Stack* s,struct Queue* q){
	struct Stack* temp=q->front;
	if(q->front==NULL){
		return;
	}
	if(q->front==q->rear){
		q->front=q->rear=NULL;
	}
	else{
		q->front=q->front->next;
	}
	free(temp);
}

//This function print the all frames with their informations
void print_frame(struct Stack* s,int c)
{	
	printf("Frame #%d\n",c+1);
	struct Layer* l;
	l=s->top;
	int info_size=2;
	int i,j;
	int counter=0;
	
	for(j=0;j<4;j++)
	{
		for(i=0;i<info_size;i++)
		{
			counter++;
			if(j==0)
			{
				if(i==0)
				{
					printf("Sender MAC address: %s , ",l->info[i]);
				}	
				else
				{
					printf("Receiver MAC address: %s\n",l->info[i]);
				}
			}
			else if(j==1)
			{
				if(i==0)
				{
					printf("Sender IP address: %s , ",l->info[i]);
				}	
				else
				{
					printf("Receiver IP address: %s\n",l->info[i]);
				}
			}
			else if(j==2)
			{
				if(i==0)
				{
					printf("Sender port number: %s , ",l->info[i]);
				}	
				else
				{
					printf("Receiver port number: %s\n",l->info[i]);
				}
			}
			else if(j==3)
			{
				if(i==0)
				{
					printf("Sender ID: %s , ",l->info[i]);
				}	
				else if(i==1)
				{
					printf("Receiver ID: %s\n",l->info[i]);
				}
				else if(i==2)
				{
					printf("Message chunk carried: %s\n",l->info[i]);
				}		
			}	
		}
		if(counter==6)
		{
			info_size=3;
		}		
		l=l->next;
	}
	printf("---------\n");
}
//This function prints information of intended stack
void print_frame_info(struct Stack* s,struct Clients c,char* i_o,char* num,int hop){
	int number=atoi(num);
	int info_size=2;
	int i,j;
	int counter=0;
	printf("Current frame #%d on the %sgoing queue of client %s\n",number,i_o,c.Id);
	for(i=0;i<number-1;i++){
		s=s->next;
	}
	struct Layer* l[4];
	struct Layer* la;
	la=s->top;
	la=pop(s,la); 	l[0]=la;
	la=pop(s,la);	l[1]=la;
	la=pop(s,la);   l[2]=la; 
	la=pop(s,la);	l[3]=la;

	printf("Carried Message: \"%s\"\n",l[3]->info[2]);
	printf("Layer 0 info: Sender ID: %s, Receiver ID: %s\n",l[3]->info[0],l[3]->info[1]);	
	printf("Layer 1 info: Sender port number: %s, Receiver port number: %s\n",l[2]->info[0],l[2]->info[1]);	
	printf("Layer 2 info: Sender Ip address: %s, Receiver Ip address: %s\n",l[1]->info[0],l[1]->info[1]);	
	printf("Layer 3 info: Sender MAC address: %s, Receiver MAC address: %s\n",l[0]->info[0],l[0]->info[1]);
	printf("Number of hops so far: %d\n",hop-1);	
}

int main(int argc, char *argv[]){
		
	struct Queue* q=(struct Queue*)malloc(sizeof(struct Queue));
	struct Stack* s=(struct Stack*)malloc(sizeof(struct Stack));
	struct Layer* la=(struct Layer*)malloc(sizeof(struct Layer));
	q->front=NULL;
	q->rear=NULL;
	s->top=NULL;
	
	int client_size=0,command_size=0,i=0,j=0,u=0,message_size=0,frame_count,max_message=atoi(argv[4]);
	char *line_clients,*info_clients,*line_commands,*info_commands,*line_route,*info_route,*route,*sender_ip_address,*receiver_ip_address,*sender_mac_address,*receiver_mac_address,*show_frame_id,*message,*stop,*succes;
	char sender_id[2];
	char receiver_id[2];
	char final_receiver[2];
	char store_clients[MAX];
	char store_commands[MAX];
	char store_route[MAX];
	char* sender_port_number=argv[5];
	char* receiver_port_number=argv[6];
	succes=(char*)malloc(sizeof(char)*3);
	
	//reading clients file
	FILE *client_file=fopen(argv[1],"r");
	//first line 
	line_clients = fgets(store_clients,300,client_file);
	client_size=atoi(line_clients);
	//allocated space for clients
	struct Clients* client_list=(struct Clients*)malloc(client_size*sizeof(struct Clients));
	//second line informations start here
	line_clients = fgets(store_clients,300,client_file);
	//in while statement I filled each clients property
	while(line_clients!=NULL)
	{
		info_clients=strtok(store_clients," ");
		strcpy(client_list[i].Id,info_clients);
		info_clients=strtok(NULL," ");
		strcpy(client_list[i].Ip_adress,info_clients);
		info_clients=strtok(NULL,"\n");
		strcpy(client_list[i].Mac_adress,info_clients);		
		client_list[i].in=NULL;
		client_list[i].out=NULL;
		i++;
		line_clients=fgets(store_clients,MAX,client_file);
	}
	fclose(client_file);
	
	//reading routing file
	int r_count=0;
	int r_count2=0;
	int c_r=0;	
	FILE *route_file=fopen(argv[2],"r");
	line_route = fgets(store_route,300,route_file);
	//in while statement I filled the each clients route table
	while(line_route!=NULL)
	{		
		if( strcmp(line_route,"-\n") == 0)
		{
			line_route = fgets(store_route,MAX,route_file);
			c_r++;
			r_count=0;
		}
		info_route=strtok(store_route," ");
		client_list[c_r].routing_table[r_count][0]=info_route[0];
		info_route=strtok(NULL,"\n");
		client_list[c_r].routing_table[r_count][1]=info_route[0];
		r_count++;
		line_route = fgets(store_route,MAX,route_file);
	}
	fclose(route_file);
	
	//reading command file
	FILE *command_file=fopen(argv[3],"r");
	line_commands = fgets(store_commands,300,command_file);
	command_size=atoi(line_commands);
	line_commands = fgets(store_commands,300,command_file);
	//end of the while I took new line(command) and handled 
	while(line_commands!=NULL)
	{	
		info_commands=strtok(store_commands," "); //I took the first word of the line and compare. if it is equal I handled the this command
		if(strcmp(info_commands,"MESSAGE") == 0)
		{	
			info_commands=strtok(NULL," ");
			sender_id[0]=info_commands[0];
			sender_id[1]='\0';
			//sender_id=info_commands;
			info_commands=strtok(NULL," ");
			//receiver_id=info_commands;	
			receiver_id[0]=info_commands[0];
			receiver_id[1]='\0';
			final_receiver[0]=info_commands[0];
			final_receiver[1]='\0';
			
			stop="-";
			
			//in this recursive function I found the route
			char* route_finder(char sender_id,char receiver_id,char* r)
			{	
				int i,j,k;
					
				if(receiver_id==sender_id || sender_id==stop[0] )
				{	
					return r;
				}
				else
				{	
					for(i=0;i<client_size;i++)
					{	
						if(client_list[i].Id[0]==sender_id)
						{
							for(j=0;r_count;j++)
							{
								if(client_list[i].routing_table[j][0]==receiver_id)
								{	
									sender_id=client_list[i].routing_table[j][1];
									char str [2];
									str[0]=sender_id;
									str[1]='\0';
									strcat(r,str);		
									return route_finder(sender_id,receiver_id,r);
								}
							}
						}	
					}	
				}
			}
			char* s2=(char *)malloc(sizeof(char));
			strcpy(s2,sender_id);
			route=route_finder(sender_id[0],receiver_id[0],s2);
			for(i=0;i<strlen(route);i++)
			{
				if(route[i]==sender_id[0])
				{
					receiver_id[0]=route[i+1];
				}
			}
			
			for(i=0;i<client_size;i++)
			{
				if(strcmp(client_list[i].Id,sender_id) == 0)
				{
					sender_ip_address=client_list[i].Ip_adress;
					sender_mac_address=client_list[i].Mac_adress;
				}
				if(strcmp(client_list[i].Id,receiver_id) == 0)
				{
					receiver_mac_address=client_list[i].Mac_adress;
				}
				if(strcmp(client_list[i].Id,final_receiver) == 0)
				{
					receiver_ip_address=client_list[i].Ip_adress;
				}
			}
		
			info_commands=strtok(NULL,"\n");
			
			//I took the message then split it with for statements which are below 		
			message_size=strlen(info_commands)-2;	
			message=(char*)malloc(message_size*sizeof(char));
		
			for(i=1;i<message_size+1;i++)
			{
				message[i-1]=info_commands[i];
			}
			message[message_size]='\0';	
			
			printf("--------------------------------------------------\n");
			printf("Command: MESSAGE %s %s #%s#\n",sender_id,final_receiver,message);
			printf("--------------------------------------------------\n");
			printf("Message to be sent: %s\n",message);
			printf("\n");
			
			frame_count=message_size/max_message;
			if(message_size%max_message!=0)
			{
				frame_count++;
			}
			int p;
			char **message_chunk=(char**)malloc(frame_count*sizeof(char*));
			for(p=0;p<frame_count;p++)
			{
				if(frame_count-1==p && message_size%max_message!=0)
				{	
					int rem=message_size-(p*max_message);
					message_chunk[p]=(char*)malloc(rem*sizeof(char));
				}
				else
				{	
					message_chunk[p]=(char*)malloc(max_message*sizeof(char));
				}
			}
			int k,l=0;	
			for(k=0;k<frame_count;k++)
			{
				if(frame_count-1==k && message_size%frame_count!=0)
				{	
					int rem=message_size-(k*max_message);
					for(p=0;p<rem;p++)
					{
						message_chunk[k][p]=message[l];
							l++;
					}
					message_chunk[k][rem]='\0';
				}
				else
				{	
					for(j=0;j<max_message;j++)
					{	
					message_chunk[k][j]=message[l];
					l++;
					}
					message_chunk[k][max_message]='\0';
				}
			}
			
			int counter=0;
			//in this while statement ý filled the each stack
			while(frame_count>counter)
			{
				int n;
				char** info=(char**) malloc (9*sizeof(char*));
				for(n=0;n<9;n++)
				{
				info[n] = (char*)malloc(MAX*10*sizeof(char));
				}
				info[0]=sender_id;
				info[1]=final_receiver;
				info[2]=message_chunk[counter];
				info[3]=sender_port_number;
				info[4]=receiver_port_number;
				info[5]=sender_ip_address;
				info[6]=receiver_ip_address;
				info[7]=sender_mac_address;
				info[8]=receiver_mac_address;
				int info_size=3;
				int o = 0;
				for(i=0;i<4;i++)
				{
					la->info = (char**) malloc (info_size*sizeof(char*));
					for(n=0;n<info_size;n++)
					{
						la->info[n] = (char*)malloc(MAX*10*sizeof(char));		
					}	
					for(j=0;j<info_size;j++)
					{
						strcpy(la->info[j],info[o++]);	
					}
					s=push(s,la,info_size);//Here I pushed the layers 
					info_size=2;
				}
				print_frame(s,counter);
				enqueue(s,q);//Here I pushed the stacks to queue
				counter++;
			}
			//I adjusted clients in or out status each clients in for statement which are below 
			for(i=0;i<client_size;i++)
			{
				if(strcmp(client_list[i].Id,sender_id) == 0)
				{	
					client_list[i].out=q;
				}	
				else if(strcmp(client_list[i].Id,final_receiver) == 0)
				{
					client_list[i].in=q;
				}   
			}
		
			for(i=1;i<strlen(route)-1;i++)
			{
				for(j=0;j<client_size;j++)
				{
					if(route[strlen(route)-1]==stop[0])
					{	
						if(route[i]==client_list[j].Id[0])
						{	
							if(i==strlen(route)-2)
							{
								client_list[j].in=q;
							}
							else
							{
								client_list[j].in=q;
								client_list[j].out=q;
							}
						}
					}
					else
					{
						if(route[i]==client_list[j].Id[0])
						{
						client_list[j].in=q;
						client_list[j].out=q;
						}	
					}
				}
			}
		}
		else if(strcmp(info_commands,"SHOW_FRAME_INFO")==0) 
		{	
			info_commands=strtok(NULL," ");
			char* temp=info_commands;
			info_commands=strtok(NULL," ");
			char* in_or_out=info_commands;
			info_commands=strtok(NULL,"\n");
			char* frame_number=info_commands;
			int hop_s=0;
			for(i=0;i<strlen(route);i++)
			{	hop_s++;
				if(temp[0]==route[i])
				{
					break;	
				}
			}
			for(i=0;i<client_size;i++)
			{
				if(strcmp(client_list[i].Id,temp) == 0)
				{	
					if(strcmp(in_or_out,"in")==0)
					{	printf("----------------------------------------------------\n");
						printf("Command: SHOW_FRAME_INFO %s %s %s\n",temp,in_or_out,frame_number);
						printf("----------------------------------------------------\n");
						if(client_list[i].in==NULL || frame_count<atoi(frame_number))
						{
							printf("No such frame\n");
						}
						else
						{   //I printed intended frames info in this function		
							print_frame_info(client_list[i].in->front,client_list[i],in_or_out,frame_number,hop_s);
						}
					}
					else if(strcmp(in_or_out,"out")==0)
					{	printf("----------------------------------------------------\n");
						printf("Command: SHOW_FRAME_INFO %s %s %s\n",temp,in_or_out,frame_number);
						printf("----------------------------------------------------\n");
						if(client_list[i].out==NULL || frame_count<atoi(frame_number))
						{
							printf("No such frame\n");
						}
						else
						{	
							print_frame_info(client_list[i].out->front,client_list[i],in_or_out,frame_number,hop_s);
						}
					}
				}	
			}	
		}
		else if(strcmp(info_commands,"SHOW_Q_INFO")==0)	//I printed clients queue info in this statement
		{
			info_commands=strtok(NULL," ");
			char* temp=info_commands;
			info_commands=strtok(NULL,"\n");
			char* in_or_out=info_commands;
			
			for(i=0;i<client_size;i++)
			{	
				if(strcmp(client_list[i].Id,temp) == 0)
				{		
					if(strcmp(in_or_out,"in")==0)
					{	printf("----------------------------------------------------\n");
						printf("Command: SHOW_Q_INFO %s %s\n",temp,in_or_out);
						printf("----------------------------------------------------\n");
						printf("Client %s Incoming Queue Status\n",temp);
						if(client_list[i].in==NULL)
						{
							printf("Current total number of frames: 0\n");
						}
						else
						{
							printf("Current total number of frames: %d\n",frame_count);
						}
					}
					if(strcmp(in_or_out,"out")==0)
					{	printf("----------------------------------------------------\n");
						printf("Command: SHOW_Q_INFO %s %s\n",temp,in_or_out);
						printf("----------------------------------------------------\n");
						printf("Client %s Outgoing Queue Status\n",temp);
						if(client_list[i].out==NULL)
						{
							printf("Current total number of frames: 0\n");
						}
						else
						{
							printf("Current total number of frames: %d\n",frame_count);
						}
					}
				}
			}
		}

		else if(strcmp(info_commands,"SEND")==0)
		{
			info_commands=strtok(NULL,"\n");
			char* temp=info_commands;
			printf("------------\n");
			printf("Command SEND %s\n",temp);
			printf("------------\n");
			int hop=0;
			
			//I printed each part of sending message part in this recursion function 
			char* send(char* route,char* sender,char*receiver,char*final,int frame_size)
			{	hop++;
				int i,j,k,m;
				char temp[2];
				
				if(hop==1 && receiver[0]==final[0])
				{	
					for(j=0;j<strlen(route);j++)
					{
						if(route[j]==receiver[0])
						{
							receiver[0]=route[j-1];
						}
					}	
				}
				
				if(strcmp(receiver,stop)==0){
					printf("Error: Unreachable destination. Packets are dropped after %d hops!\n",hop-1);
					return final;
				}
				if(strcmp(receiver,final)==0)
				{   
					if(strlen(route)==2)
					{
					hop--;
					}
					printf("A message received by client %s from client %s after a total of %d hops\n",final,sender,hop);
					printf("Message: %s\n",message);
					return final;	
				}
				else{
					printf("A message received by client %s, but intended for client %s. Forwarding...\n",receiver,final);
					for(j=0;j<strlen(route);j++)
					{
						if(route[j]==receiver[0])
						{
							m=j+1;
						}
					}
					for(i=0;i<client_size;i++)
					{
						if(client_list[i].Id[0]==route[m]){
							temp[0]=route[m];
							temp[1]='\0';
						}	
					}
					for(i=0;i<client_size;i++)
					{	
						for(j=0;j<client_size;j++)
						{	
							if(strcmp(client_list[i].Id,receiver)==0 && strcmp(client_list[j].Id,temp)==0)
							{								
								for(k=0;k<frame_size;k++)
								{
									printf("        Frame #%d MAC address change: New sender MAC %s, new receiver MAC %s\n",k+1,client_list[i].Mac_adress,client_list[j].Mac_adress);
								}	
							}	 					
						}
					}
				}
					receiver[0]=route[m];
					return send(route,sender,receiver,final,frame_size);
			}
			if(sender_id[0]==temp[0]){
			char* rtrn=send(route,sender_id,receiver_id,final_receiver,frame_count);
			}
			else{
				printf("invalid situation Sender is %s not %s \n",sender_id,temp);
			}
		}
		
		else if(strcmp(info_commands,"PRINT_LOG")==0)
		{	
			char* activity=(char*)malloc(sizeof(char)*20);
			char* activity2=(char*)malloc(sizeof(char)*20);
			int log_count,i,j,hop;
			hop=0;
			log_count=0;
			info_commands=strtok(NULL,"\n");
			char* temp=info_commands;
			time_t t = time(NULL);
			struct tm tm = *localtime(&t);
			printf("-------------------\nCommand: PRINT_LOG %s\n----------------------\nClient %s Logs:\n----------------------\n",temp,temp);
			for(i=0;i<strlen(route);i++)
			{
				if(temp[0]==route[0])
				{
					log_count=1;
					strcpy(activity,"Message Forwarded");
				}
				else if(temp[0]==route[strlen(route)-1])
				{	
					log_count=1;
					strcpy(activity,"Message Received");
				}
				else if(temp[0]==route[strlen(route)-1] || (route[strlen(route)-1]==stop[0] && temp[0]==route[strlen(route)-2]))
				{
					log_count=1;
					strcpy(activity,"Message Received");	
				}
				else if(temp[0]==route[i])
				{
					log_count=2;
					strcpy(activity2,"Message Forwarded");
					strcpy(activity,"Message Received");
				}
			}
			for(i=0;i<strlen(route);i++)
			{	hop++;
				if(temp[0]==route[i])
				{
					break;	
				}
			}
			for(i=0;i<log_count;i++)
			{
				printf("Log Entry #%d:\n",i+1);
				printf("Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
				printf("Message: %s\n",message);
				printf("Number of frames %d\n",frame_count);
				printf("Number of hops: %d\n",hop-1);
				printf("Sender ID: %s\n",sender_id);
				printf("Receiver ID: %s\n",final_receiver);
				if(log_count==2 && i==0)
				{
					printf("Activity: %s\n",activity);
				}
				else if(log_count==2 && i==1)
				{
					printf("Activity: %s\n",activity2);
				}
				else if(log_count==1)
				{
					printf("Activity %s\n",activity);
				}
				for(j=0;j<strlen(route);j++)
				{
					if(temp[0]==route[j] && i==0)
					{
						if(route[j-1]==stop[0])
						{
							strcpy(succes,"NO");
						}
						else
						{
							strcpy(succes,"YES");
						}
					}
					if(temp[0]==route[j] && i==1)
					{
						if(route[j+1]==stop[0])
						{
							strcpy(succes,"NO");
						}
						else
						{
							strcpy(succes,"YES");
						}
					}
				}
				printf("Succes: %s\n",succes);
				printf("-----------------\n");
			}
		}
		else
		{	char* first=info_commands;
			char* rest=strtok(NULL,"\n");
			printf("Command: %s %s\n",first,rest);
			printf("----------------------------\n");
			printf("Invalid command\n");	
		
		}
		line_commands = fgets(store_commands,300,command_file);
	}	
	fclose(command_file);
	free(s);
	free(la);
	free(q);
	return 0;
}
