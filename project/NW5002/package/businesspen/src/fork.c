/* *  fork_test.c *  version 1 *  Created on: 2010-5-29 *      Author: wangth */
#include <unistd.h>
#include <stdio.h> 

int main () 
{ 	
    pid_t fpid; //fpid��ʾfork�������ص�ֵ	
    int count=0;	

    fpid=fork(); 	

	if (fpid < 0) 		
	    printf("error in fork!"); 	
    else if (fpid == 0) 
	{	
	    while(count < 10)
	    {
	        printf("i am the child process, my process id is %d\n",getpid()); 		
	        printf("���ǵ��Ķ���\n");//��ĳЩ����˵���Ŀ��Ÿ�ֱ�ס�
	        fflush(stdout);
	        count++;	
		    sleep(5);
		}
	}
    else 
	{	
	    while(count < 10)
	    {
	        printf("i am the parent process, my process id is %d\n",getpid()); 		
	        printf("���Ǻ�������\n");
			fflush(stdout);
	        count++;
			sleep(5);
	    }	    
	}
	printf("ͳ�ƽ����: %d\n",count);
return 0;
}

