#ifndef _PPC_LIST_H_
#define _PPC_LIST_H_

#ifdef __cplusplus
extern "C"{
#endif
#include "base.h"
#include "list.h"

typedef struct _dl_ppc_fd_list
{
	struct dl_list head;
	int has_init;
	pthread_mutex_t mutex;
}dl_ppc_fd_list;

enum{
	fd_type_open = 1,
	fd_type_fopen = 2,
	fd_type_opendir = 3,
}fd_type;

typedef struct _fd_info
{
	int v_fd;		//�����fd
	int type;		//fd����
	int socket_fd;	//socket��fd
	int file_fd;	//��ʵ���ļ�fd
	char *path;		//�ļ�·��
	off_t offset;	//�ļ�ƫ��
	off_t file_len; //�ļ�����
	int 	flag;	//���ļ��ı�־
	mode_t 	mode;	//���ļ���ģʽ
	char	f_mode[8];//fopen��mode
	struct dirent **p_data;
	int 	file_count;
	struct dl_list next;
}fd_info;

int init_ppc_fd_list();
int add_info_for_ppc_fd_list(fd_info *p_fd_info);
int del_info_for_ppc_fd_list(int v_fd);
int inc_offset_for_ppc_fd_list(int v_fd, off_t offset);
int set_offset_for_ppc_fd_list(int v_fd, off_t offset);
int get_socket_fd_from_ppc_fd_list(int v_fd);
int ch_socket_fd_for_ppc_fd_list(int v_fd, int socket_fd);
int get_info_from_ppc_fd_list(int v_fd, fd_info **p_info);
int free_ppc_fd_list();

#ifdef __cplusplus
}
#endif

#endif
