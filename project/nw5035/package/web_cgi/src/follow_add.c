/*
 * =============================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  hidisk server module.
 *
 *        Version:  1.0
 *        Created:  2015/3/19 10:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Oliver (), 515296288jf@163.com
 *   Organization:  
 *
 * =============================================================================
 */

#include "msg.h"

int _handle_client_json_req(ClientTheadInfo *client_info)
{
 //   return api_process(client_info);
}


void main()
{
		char ret_buf[RET_BUF_LEN];
		char code[CODE_LEN]="\0";
		char sid[SID_LEN]="\0";
		char fw_sid[SID_LEN]="\0";
		char type[32]="\0";
		char pid[1024]="\0";
		char tmp_buf[256]="\0";
		char tmp_vid[VID_LEN_33]="\0";
		char ext[128]="\0";
		char tag[512]="\0";
		int i,j,k;
		char *web_str=NULL;
		int ret=0;

		char uci_option_str[UCI_BUF_LEN]="\0";
		ctx=uci_alloc_context();
		
		strcpy(uci_option_str,"system.@system[0].sid");			//name
		uci_get_option_value(uci_option_str,fw_sid);
		memset(uci_option_str,'\0',UCI_BUF_LEN);

		
		printf("Content-type:text/plain\r\n\r\n");

		if((web_str=GetStringFromWeb())==NULL)
		{
			sprintf(ret_buf,"%s","{\"status\":0,\"data\":{},\"errorCode\":4,\"errorMessage\":\"Can not get any parameters.\"}");
			printf("%s",ret_buf);
			fflush(stdout);
			uci_free_context(ctx);
			return ;

		}
		processString(web_str,SID,sid);		
		
		processString(web_str,CODE,code);		
		
		processString(web_str,PID,pid);	
		
		processString(web_str,EXT,ext);	
		
		processString(web_str,TAG,tag);	

		char *pExt=urlDecode(ext);
		char *pTag=urlDecode(tag);		
		p_debug("ext==%s",ext);
		p_debug("pext==%s",pExt);
		p_debug("sid==%s",sid);
		p_debug("fwsid==%s",fw_sid);

		//{"follow_add":[{"pid":"1111","ext":{}},{"pid":"2222","ext":{}}]}
		//{"follow_add":{"pid":"","ext":{}}}
		if(!strcmp(sid,fw_sid)){//�ǹ���Ա
			if(pid[0]=='\0'){
				sprintf(ret_buf,"%s","{\"status\":0,\"data\":{},\"errorCode\":4,\"errorMessage\":\"pid is empty\"}");
				goto exit;
			}

			if(pExt[0]=='\0'){
				sprintf(ret_buf,"%s","{\"status\":0,\"data\":{},\"errorCode\":4,\"errorMessage\":\"ext is empty\"}");
				goto exit;
			}
			
			if(pTag[0]=='\0'){
				sprintf(ret_buf,"%s","{\"status\":0,\"data\":{},\"errorCode\":4,\"errorMessage\":\"tag is empty\"}");
				goto exit;
			}

			sprintf(buf,"{\"follow_add\":{\"pid\":\"%s\",\"ext\":%s,\"tag\":%s}}",pid,pExt,pTag);
			
			#if 0
			{
			
				sprintf(buf,"{\"%s\":{\"%s\":[",FOLLOW_ADD,PID);

				//char *str=strstr(vid,',');
	//			for(i=0,j=0,k=0;i<strlen(vid);i++)
				for(i=0,j=0,k=0;i<strlen(pid);i++)
					{
						if(pid[i]==',')
						{ 
							j++;
							//strncpy(tmp_vid,vid+i,(i+1));
							sprintf(tmp_buf,"\"%s\",",tmp_vid);
							strcat(buf,tmp_buf);
							memset(tmp_vid,0,strlen(tmp_vid));
							k=0;
						}
						else {
							tmp_vid[k]=pid[i];
							k++;
						}
				}

				sprintf(tmp_buf,"\"%s\"",tmp_vid);
				strcat(buf,tmp_buf);
				strcat(buf,"]}}");
			}
			//else{
			//	sprintf(buf,"{\"%s\":{\"%s\":\"%s\"}}",TASK_REMOVE,VID,);
			//}
			#endif
			p_debug("buf=====%s",buf);
		
			
			ret = notify_server();
			if(ret <= 0){//ͨѶ����
				sprintf(ret_buf,"%s","{\"status\":0,\"data\":{},\"errorCode\":13,\"errorMessage\":\"Communication Error with dm_letv\"}");
			}else//���յ���Ϣ
				printf("%s",p_client_info.recv_buf);
				fflush(stdout);

				free(p_client_info.recv_buf);
				free(web_str);
				free(pExt);
				uci_free_context(ctx);
				return ;

		}else{
			sprintf(ret_buf,"%s","{\"status\":0,\"data\":{},\"errorCode\":1,\"errorMessage\":\"Not Admin\"}");
		} 
exit:
		printf("%s",ret_buf);
		p_debug(ret_buf);
		fflush(stdout);
		free(web_str);
		free(pExt);		
		uci_free_context(ctx);
		return ;
}
