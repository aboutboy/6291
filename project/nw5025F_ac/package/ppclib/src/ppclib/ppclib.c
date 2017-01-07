/*
 * =============================================================================
 *
 *       Filename:  ppclib.c
 *
 *    Description:  file cache interface
 *
 *        Version:  1.0
 *        Created:  2016/10/14 14:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Oliver (), 515296288jf@163.com 18503096207
 *   Organization: longsys 
 *
 * =============================================================================
 */
#include "ppclib.h"
#include "ppc_file_client.h"
#include "my_debug.h"
#include "assert.h"
#include "ppc_fd_list.h"
#include "ppc_token_list.h"
#include "ppc_deal_path.h"


/**
  * function:��ȡ��汾��Ϣ
  * ppc_version: ��汾��Ϣ
  * return:0 sucess,!0 failed
  */
int ppc_get_pkg_version(pkg_version *pkg_version_info)
{
	if(pkg_version_info == NULL){
		return FAIL;
	}

	strcpy(pkg_version_info->pkg_name, PKG_NAME);
	strcpy(pkg_version_info->pkg_version, PKG_VERSION);

	return SUCCESS;
}


/**
  * function:ppc lib��ʼ��
  * return:0 sucess,!0 failed
  */
int ppc_initialise()
{
	int res = 0;
	res = init_ppc_fd_list();
	if(0 != res){
		return FAIL;
	}
	
	res = init_ppc_token_list();
	if(0 != res){
		return FAIL;
	}

	res = InitLinklist();
	if(res)
	{
		DMCLOG_E("InitLinklist failed...");
		return FAIL;
	}
	
	return SUCCESS;
}


/**
  * function:ppc lib����ʼ��
  * return:0 sucess,!0 failed
  */
int ppc_uninitialise()
{
	int res = 0;
	res = free_ppc_fd_list();
	if(0 != res){
		return FAIL;
	}	

	res = free_ppc_token_list();
	if(0 != res){
		return FAIL;
	}

	ReleaseLinklist();
	
	return SUCCESS;
}


/**
 * function:�˺�ע��
 * param:
 * username �û���
 * password ����
 * return:0 sucess,!0 failed
 */
ERROR_CODE_PPC ppc_register(char* username,char* password)
{
    int res = handle_register_task(username,password);
    if(res != 0)
    {
        return FAIL;
    }
	return SUCCESS;
}


/**
 * function:�˺�ע�ᣬ֧��ͬ���û���˵�¼
 * param:
 * username �û���
 * password ����
 *  return:0 sucess,!0 failed
 */
ERROR_CODE_PPC ppc_login(char* username, char* password, _int64_t *token)
{
    int res = 0;
	res = handle_login_task(username,password,token);
    if(res != 0){
        return USERNAME_NOT_FOUND;
    }

	token_info *login_token_info = (token_info *)calloc(1, sizeof(token_info));
	if(login_token_info == NULL){
		return FAIL;
	}

	login_token_info->token = *token;
	login_token_info->work_dir = strdup("/");
	res = add_info_for_ppc_token_list(login_token_info);
	if(0 != res){
		return FAIL;
	}	

	return SUCCESS;
}

/**
 * function:�û��˳�
 * param:
 * token �û���ʶ
 * return:0 sucess,!0 failed
 */
ERROR_CODE_PPC ppc_logout(_int64_t token)
{
    int res = 0;
	res = handle_logout_task(token);
    if(res != 0){
        return USERNAME_NOT_FOUND;
    }
	
	res = del_info_for_ppc_token_list(token);
	if(0 != res){
		return FAIL;
	}
	
	return SUCCESS;
}

/**
 * ��ĳ�ַ�ʽ���ļ�
 * param��
 * path���ļ�·��
 * mode����ģʽ
 * ��r�� ��ֻ����ʽ���ļ������ļ�������ڡ�
��r+�� �Կɶ�д��ʽ���ļ������ļ�������ڡ�
��rb+�� ��д��һ���������ļ��������д���ݣ��ļ�������ڡ�
��w�� ��ֻд�ļ������ļ��������ļ�������Ϊ0�������ļ����ݻ���ʧ�����ļ��������������ļ���
��w+�� �򿪿ɶ�д�ļ������ļ��������ļ�������Ϊ�㣬�����ļ����ݻ���ʧ�����ļ��������������ļ���
��a�� �Ը��ӵķ�ʽ��ֻд�ļ������ļ������ڣ���Ὠ�����ļ�������ļ����ڣ�д������ݻᱻ�ӵ��ļ�β�����ļ�ԭ�ȵ����ݻᱻ��������EOF��������
��a+�� �Ը��ӷ�ʽ�򿪿ɶ�д���ļ������ļ������ڣ���Ὠ�����ļ�������ļ����ڣ�д������ݻᱻ�ӵ��ļ�β�󣬼��ļ�ԭ�ȵ����ݻᱻ������ ��ԭ����EOF����������
��wb�� ֻд�򿪻��½�һ���������ļ���ֻ����д���ݡ�
��wb+�� ��д�򿪻���һ���������ļ����������д
��wx�� �����ı��ļ�,ֻ����д������.[C11]
��wbx�� ����һ���������ļ�,ֻ����д������.[C11]
��w+x�� ����һ���ı��ļ�,�����д.[C11]
��wb+x�� ����һ���������ļ�,�����д.[C11]
��w+bx�� ��"wb+x"��ͬ[C11]
��rt�� ֻ����һ���ı��ļ���ֻ���������
������wt�� ֻд�򿪻���һ���ı��ļ���ֻ����д����
������at�� ׷�Ӵ�һ���ı��ļ��������ļ�ĩβд����
������rb�� ֻ����һ���������ļ���ֻ���������
������wb�� ֻд�򿪻���һ���������ļ���ֻ����д����
������ab�� ׷�Ӵ�һ���������ļ��������ļ�ĩβд����
������rt+�� ��д��һ���ı��ļ����������д
������wt+�� ��д�򿪻���һ���ı��ļ��������д
������at+�� ��д��һ���ı��ļ���������������ļ�ĩ׷������
������rb+�� ��д��һ���������ļ����������д
������ab+�� ��д��һ���������ļ���������������ļ�ĩ׷������
 *
 * token����¼ʱ���ص�token
 */
FILE *ppc_fopen(const char *path, const char *mode, _int64_t token)
{
	int ret = 0;
	char *full_path = NULL;
	
	if(path == NULL || mode == NULL){
		DMCLOG_E("para is null");
		return NULL;
	}

	ret = create_new_full_path(path, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return NULL;
	}
	
	PFILE *file_info = (PFILE *)calloc(1,sizeof(PFILE));
	if(file_info == NULL){
		DMCLOG_E("malloc error");
		safe_free(full_path);
		return NULL;
	}
	
    file_info->token = token;
	strcpy(file_info->mode,mode);
	file_info->offset = 0;
	file_info->srcPath = full_path;
	ret = handle_fopen_task(file_info);
	if(ret != 0){
		DMCLOG_E("handle fopen error");
		safe_free(file_info->srcPath);
		safe_free(file_info);
		return NULL;
	}

	fd_info *open_fd_info = (fd_info *)calloc(1, sizeof(fd_info));
	if(open_fd_info == NULL){
		DMCLOG_E("malloc open_fd_info fail");
		safe_free(file_info->srcPath);
		safe_free(file_info);
		return NULL;
	}

	open_fd_info->socket_fd = file_info->fd;
	open_fd_info->path = strdup(file_info->srcPath);
	open_fd_info->offset = file_info->offset;
	open_fd_info->file_len = file_info->length;
	open_fd_info->type = fd_type_fopen;
	memcpy(open_fd_info->f_mode, file_info->mode, strlen(file_info->mode));
	ret = add_info_for_ppc_fd_list(open_fd_info);
	if(ret){
		DMCLOG_E("add_info_for_ppc_fd_list fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info->srcPath);
		safe_free(file_info);
		return NULL;
	}

	int v_fd = open_fd_info->v_fd;
	//FILE *fp = (FILE *)calloc(1, sizeof(FILE));
	FILE *fp = (FILE *)v_fd;

	if(fp == NULL){
		del_info_for_ppc_fd_list(v_fd);
		safe_free(file_info->srcPath);
		safe_free(file_info);
	}

	//fp->_fileno = open_fd_info->v_fd;
	safe_free(file_info->srcPath);
	safe_free(file_info);

	return fp;
}

/**
 * �ļ���ȡ����
 */
size_t ppc_fread(void *buffer, size_t size, size_t count, FILE *fp)
{
	int ret = 0;
	int v_fd = (int)fp;
	int socket_fd = get_socket_fd_from_ppc_fd_list(v_fd);
	if(socket_fd < 0){
		return FAIL;
	}

	ssize_t read_len = handle_fread_task(socket_fd, count, buffer);
	if(read_len > 0){
		ret = inc_offset_for_ppc_fd_list(v_fd, (off_t)read_len);
		if(ret){
			return FAIL;
		}
	}

	return read_len;
}

/**
 * �ļ�д����
 */
size_t ppc_fwrite(const void* buffer, size_t size, size_t count, FILE *fp)
{
	int ret = 0;
	int v_fd = (int)fp;
	int socket_fd = get_socket_fd_from_ppc_fd_list(v_fd);
	if(socket_fd < 0){
		return FAIL;
	}
	
	ssize_t write_len = handle_fwrite_task(socket_fd,count,buffer);
	if(write_len > 0){
		ret = inc_offset_for_ppc_fd_list(v_fd, (off_t)write_len);
		if(ret){
			return FAIL;
		}
	}

	return write_len;
}

/**
 * �ļ�ָ��ƫ��
 * fromwhere��ƫ����ʼλ�ã��ļ�ͷ0(SEEK_SET)����ǰλ��1(SEEK_CUR)���ļ�β2(SEEK_END)��Ϊ��׼��ƫ��offset��ָ��ƫ���������ֽڵ�λ�á�
 */
off_t ppc_fseek(FILE *fp, off_t offset, int fromwhere, _int64_t token)
{
	DMCLOG_D("start ppc_fseek");
	int ret = 0;
	int v_fd = 0;
	off_t seek_offset = 0;
	fd_info *open_fd_info = NULL;
	
	if(fp == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	v_fd = (int)fp;
	
	ret = get_info_from_ppc_fd_list(v_fd, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail");
		return FAIL;
	}
	DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);
	
	PFILE *file_info = (PFILE *)calloc(1,sizeof(PFILE));
	if(file_info == NULL){
		DMCLOG_E("calloc file_info error");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return FAIL;
	}

	if(fromwhere == SEEK_SET){
		seek_offset = offset;
	}
	else if(fromwhere == SEEK_CUR){
		seek_offset = open_fd_info->offset + offset;
	}
	else if(fromwhere == SEEK_END){
		seek_offset = open_fd_info->file_len + offset;
	}
	else{
		DMCLOG_E("argument error");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	if(seek_offset > open_fd_info->file_len){
		DMCLOG_E("seek_offset(%lld) is longer than file length(%lld)!!!", seek_offset, open_fd_info->file_len);
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	file_info->offset = seek_offset;
	file_info->srcPath = open_fd_info->path;
	memcpy(file_info->mode, open_fd_info->f_mode, strlen(open_fd_info->f_mode));
    file_info->token = token;
	ret = handle_fopen_task(file_info);
	if(ret != 0){
		DMCLOG_E("handle open error");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	//����ɵ�fd
	int socket_fd_old = open_fd_info->socket_fd;
	
	ret = ch_socket_fd_for_ppc_fd_list(v_fd, file_info->fd);
	if(ret){
		DMCLOG_E("change socket fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}
	//���ܵ���ppc_close
	close(socket_fd_old);

	ret = set_offset_for_ppc_fd_list(v_fd, file_info->offset);
	if(ret){
		DMCLOG_E("set_offset_for_ppc_fd_list fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}	

	safe_free(open_fd_info->path);
	safe_free(open_fd_info);
	safe_free(file_info);

	return seek_offset;	
}

off_t ppc_ftell(FILE *fp, _int64_t token)
{
	int ret = 0;
	int v_fd = 0;
	off_t offset = 0;
	fd_info *open_fd_info = NULL;
	
	if(fp == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	v_fd = (int)fp;
	
	ret = get_info_from_ppc_fd_list(v_fd, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail");
		return FAIL;
	}
	DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);

	offset = open_fd_info->offset;

	safe_free(open_fd_info->path);
	safe_free(open_fd_info);

	return offset;
}

/**
 * �ر��ļ�ָ��
 */
int ppc_fclose(FILE *fp)
{
	if(fp == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}
	int v_fd = (int)fp;
	if(v_fd > 0){
		int socket_fd = get_socket_fd_from_ppc_fd_list(v_fd);
		if(socket_fd > 0){
			close(socket_fd);
			if(del_info_for_ppc_fd_list(v_fd) < 0){
				return FAIL;
			}
			else{
				//safe_free(fp);
				return SUCCESS;
			}	
		}
		else{
			return FAIL;
		}
	}
	else{
		return FAIL;
	}
}

/*
 *��Ŀ¼
 */
DIR *ppc_opendir(const char *path, _int64_t token)
{
	int ret = 0;
	struct dirent **p_data;
	char *full_path = NULL;
	int file_count = 0;
	
	if(path == NULL){
		DMCLOG_E("para is null");
		return NULL;
	}

	ret = create_new_full_path(path, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return NULL;
	}

    file_count = handle_opendir_task(full_path, &p_data, token);
	if(file_count < 0 || p_data == NULL){
		safe_free(full_path);
        return NULL;
    }	

	fd_info *open_dir_info = (fd_info *)calloc(1, sizeof(fd_info));
	if(open_dir_info == NULL){
		safe_free(full_path);
		DMCLOG_E("malloc open_fd_info fail");
		return NULL;
	}

	open_dir_info->path = strdup(full_path);
	open_dir_info->p_data = p_data;
	open_dir_info->offset = 0;
	open_dir_info->type = fd_type_opendir;
	open_dir_info->file_count = file_count;
	ret = add_info_for_ppc_fd_list(open_dir_info);
	if(ret){
		DMCLOG_E("add_info_for_ppc_fd_list fail");
		safe_free(full_path);
		safe_free(open_dir_info->path);
		safe_free(open_dir_info);
		return NULL;
	}	

	//PPC_DIR *p_dir = (PPC_DIR *)calloc(1, sizeof(PPC_DIR));
	int v_fd = open_dir_info->v_fd;
	DIR *p_dir = (DIR *)v_fd;
	if(p_dir == NULL){
		safe_free(full_path);
		del_info_for_ppc_fd_list(v_fd);
	}

	//p_dir->__entry_ptr = v_fd;	
	safe_free(full_path);
    return p_dir;
}

/*
 *�����ļ�������Ŀ¼
 */
DIR *ppc_fdopendir(int fd, _int64_t token)
{
	int ret = 0;
	char *full_path = NULL;
	fd_info *open_fd_info = NULL;
	struct dirent **p_data = NULL;
	int file_count = 0;

	ret = get_info_from_ppc_fd_list(fd, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail");
		return NULL;
	}
	DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);
	full_path = strdup(open_fd_info->path);
	safe_free(open_fd_info->path);
	safe_free(open_fd_info);

	file_count = handle_opendir_task(full_path, &p_data, token);
    if(file_count < 0 || p_data == NULL){
		safe_free(full_path);
        return NULL;
    }

	fd_info *open_dir_info = (fd_info *)calloc(1, sizeof(fd_info));
	if(open_dir_info == NULL){
		safe_free(full_path);
		DMCLOG_E("malloc open_fd_info fail");
		return NULL;
	}

	open_dir_info->path = strdup(full_path);
	open_dir_info->p_data = p_data;
	open_dir_info->offset = 0;
	open_dir_info->type = fd_type_opendir;
	open_dir_info->file_count = file_count;
	ret = add_info_for_ppc_fd_list(open_dir_info);
	if(ret){
		DMCLOG_E("add_info_for_ppc_fd_list fail");
		safe_free(full_path);
		safe_free(open_dir_info->path);
		safe_free(open_dir_info);
		return NULL;
	}	

	//PPC_DIR *p_dir = (PPC_DIR *)calloc(1,sizeof(PPC_DIR));
	int v_fd = open_dir_info->v_fd;
	DIR *p_dir = (DIR *)v_fd;
	if(p_dir == NULL){
		safe_free(full_path);
		del_info_for_ppc_fd_list(v_fd);
	}

	//p_dir->__entry_ptr = v_fd;	
	safe_free(full_path);
    return p_dir;
}

/*
 *��ȡĿ¼
 */
struct dirent *ppc_readdir(DIR *dp, _int64_t token)
{
	int v_fd = 0;
	int ret = 0;
	fd_info *open_fd_info = NULL;

	if(dp == NULL){
		return NULL;
	}

	//PPC_DIR *p_dir = (PPC_DIR *)dp;
	v_fd = (int)dp;

	ret = get_info_from_ppc_fd_list(v_fd, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->p_data == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail(ret %d)", ret);
		return NULL;
	}

	DMCLOG_D("open_fd_info->offset: %lld, open_fd_info->file_count: %d", open_fd_info->offset, open_fd_info->file_count);
	if(open_fd_info->offset >= open_fd_info->file_count){
		DMCLOG_D("read end");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return NULL;	
	}

	struct dirent *dirp = NULL;
    dirp = handle_readdir_task(open_fd_info->p_data, open_fd_info->offset);
	if(dirp == NULL){
		DMCLOG_D("handle_readdir_task fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return NULL;
	}

	ret = inc_offset_for_ppc_fd_list(v_fd, 1);
	if(ret){
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return NULL;
	}

	safe_free(open_fd_info->path);
	safe_free(open_fd_info);
	
	return dirp;	
}

/*
 *����Ŀ¼ָ��
 */
void ppc_rewinddir(DIR *dp, _int64_t token)
{
	int v_fd = 0;
	int ret = 0;

	if(dp == NULL){
		return ;
	}

	//PPC_DIR *p_dir = (PPC_DIR *)dp;
	v_fd = (int)dp;

	ret = set_offset_for_ppc_fd_list(v_fd, 0);
	if(ret){
		DMCLOG_E("set_offset_for_ppc_fd_list fail");
		return ;
	}

	DMCLOG_E("set_offset_for_ppc_fd_list success");
	return ;
}

/*
 *�ر�Ŀ¼
 */
int ppc_closedir(DIR *dp, _int64_t token)
{
	int v_fd = 0;
	int ret = 0;

	if(dp == NULL){
		return FAIL;
	}

	//PPC_DIR *p_dir = (PPC_DIR *)dp;
	v_fd = (int)dp;

	del_info_for_ppc_fd_list(v_fd);
	//safe_free(dp);
	
	return SUCCESS;
}

/*
 *��ȡĿ¼λ��
 */
off_t ppc_telldir(DIR *dp, _int64_t token)
{
	int v_fd = 0;
	int ret = 0;

	if(dp == NULL){
		return FAIL;
	}

	//PPC_DIR *p_dir = (PPC_DIR *)dp;
	v_fd = (int)dp;

	return get_offset_fd_from_ppc_fd_list(v_fd);
}

/*
 *����ָ��Ŀ¼λ��
 */
void ppc_seekdir(DIR *dp, off_t loc, _int64_t token)
{
	int v_fd = 0;
	int ret = 0;

	if(dp == NULL){
		return ;
	}

	//PPC_DIR *p_dir = (PPC_DIR *)dp;
	v_fd = (int)dp;

	ret = set_offset_for_ppc_fd_list(v_fd, loc);
	if(ret){
		DMCLOG_E("set_offset_for_ppc_fd_list fail");
		return ;
	}

	DMCLOG_E("set_offset_for_ppc_fd_list success");
	return ;
}

/*
 *����Ŀ¼
 */
int ppc_mkdir(const char *dirname, mode_t mode, _int64_t token)
{
	int ret = 0;
	char *full_path = NULL;
	
	if(dirname == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(dirname, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}	

	// TODO: ȱ��mode����
	ret = handle_mkdir_task(full_path, mode, token);
	if(ret != 0){
		DMCLOG_D("handle_mkdir_task fail");
		safe_free(full_path);
		return FAIL;
	}

	safe_free(full_path);
	return SUCCESS;
}

/*
 *ɾ��Ŀ¼
 */
int ppc_rmdir(const char *pathname, _int64_t token)
{
	int ret = 0;
	char *full_path = NULL;
	
	if(pathname == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(pathname, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}	
	
	//�ļ��������ļ�ʱ��rmdirӦ����ʧ��
	ret = handle_rmdir_task(full_path, token);
	if(ret != 0){
		DMCLOG_D("handle_rmdir_task fail");
		safe_free(full_path);
		return FAIL;
	}

	safe_free(full_path);
	return SUCCESS;
}

/*
 *ͨ���ļ�·����ȡ�ļ���Ϣ
 */
int ppc_stat(const char *file_name, struct stat *buf, _int64_t token)
{
	int ret = 0;
	char *full_path = NULL;
	
	if(file_name == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(file_name, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}

    ret = handle_stat_task(full_path, buf, token);
    if(ret != SUCCESS){
        DMCLOG_E("stat %s error", full_path);
		safe_free(full_path);
        return FAIL;
    }
	safe_free(full_path);
    return SUCCESS;
}


/*
  *ͨ���ļ���������ȡ�ļ���Ϣ
  */
int ppc_fstat(int fildes, struct stat *buf, _int64_t token)
{
	int ret = 0;
	fd_info *open_fd_info = NULL;
	ret = get_info_from_ppc_fd_list(fildes, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail");
		return FAIL;
	}
	DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);

	ret = handle_stat_task(open_fd_info->path, buf, token);
    if(ret != SUCCESS){
		DMCLOG_E("fstat %s(%d) error", open_fd_info->path, fildes);
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
        return FAIL;
    }

	safe_free(open_fd_info->path);
	safe_free(open_fd_info);
    return SUCCESS;
}

/*
  *ͨ���ļ�·����ȡ�ļ���Ϣ(�������������ļ�)
  */
int ppc_lstat(const char *path, struct stat *buf, _int64_t token)
{
	int ret = 0;
	char *full_path = NULL;
	
	if(path == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(path, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}
	
	ret = handle_stat_task(full_path, buf, token);
    if(ret != SUCCESS){
        DMCLOG_E("stat %s error", full_path);
		safe_free(full_path);
        return FAIL;
    }

	safe_free(full_path);
    return SUCCESS;
}

/*
 *���ļ�
 * pathname:���򿪵��ļ���
 * flags:
 O_RDONLY��ֻ����ʽ���ļ�
 O_WRONLY��ֻд��ʽ���ļ�
 O_RDWR�Կɶ�д��ʽ���ļ���
 �������ֱ�־λ�ǻ���ģ�Ҳ���ǲ���ͬʱʹ�ã����������еı�־λ����OR(|)�������ϡ�
 O_CREAT�����򿪵��ļ����������Զ��������ļ���
 O_EXCL���O_CREATҲ�����ã���ָ���ȥ����ļ��Ƿ���ڡ��ļ����������������ļ������򽫵��´��ļ����󡣴��⣬��O_CREAT��O_EXCLͬʱ���ã��������򿪵��ļ�Ϊ�������ӣ������ļ�ʧ�ܡ�
 O_NOCTTY������򿪵��ļ�Ϊ�ն˻��豸ʱ���򲻻Ὣ���ն˻����ɽ��̿����ն˻���
 O_TRUNC���ļ����ڲ����Կ�д�ķ�ʽ��ʱ���˱�־λ�����ļ�������Ϊ0����ԭ�����ڸ��ļ�������Ҳ����ʧ��
 O_APPEND����д�ļ�ʱ����ļ�β��ʼ�ƶ���Ҳ������д������ݻ��Ը��ӵķ�ʽ���뵽�ļ����档
 O_NONBLOCK�Բ�����ϵķ�ʽ���ļ���Ҳ���������������ݶ�ȡ��ȴ��������������ؽ���֮�С�
 O_NDELAYͬO_NONBLOCK��
 O_SYNC��ͬ���ķ�ʽ���ļ���
 O_NOFOLLOW�������pathname��ָ���ļ�Ϊһ�������ӣ��������ļ�ʧ�ܡ�
 O_DIRECTORY�������pathname��ָ���ļ�����ΪһĿ¼���������ļ�ʧ�ܡ�
 * mode:
 S_IRWXU��00700Ȩ�ޣ�������ļ������߾��пɶ�����д����ִ�е�Ȩ�ޡ�
 S_IRUSR��S_IREAD��00400Ȩ�ޣ�������ļ������߾��пɶ�ȡ��Ȩ�ޡ�
 S_IWUSR��S_IWRITE��00200Ȩ�ޣ�������ļ������߾��п�д���Ȩ�ޡ�
 S_IXUSR��S_IEXEC��00100Ȩ�ޣ�������ļ������߾��п�ִ�е�Ȩ�ޡ�
 S_IRWXG00070Ȩ�ޣ�������ļ��û�����пɶ�����д����ִ�е�Ȩ�ޡ�
 S_IRGRP00040Ȩ�ޣ�������ļ��û�����пɶ���Ȩ�ޡ�
 S_IWGRP00020Ȩ�ޣ�������ļ��û�����п�д���Ȩ�ޡ�
 S_IXGRP00010Ȩ�ޣ�������ļ��û�����п�ִ�е�Ȩ�ޡ�
 S_IRWXO00007Ȩ�ޣ����������û����пɶ�����д����ִ�е�Ȩ�ޡ�
 S_IROTH00004Ȩ�ޣ����������û����пɶ���Ȩ��
 S_IWOTH00002Ȩ�ޣ����������û����п�д���Ȩ�ޡ�
 S_IXOTH00001Ȩ�ޣ����������û����п�ִ�е�Ȩ�ޡ�
 */
int ppc_open(const char *pathname, int flags, mode_t mode, _int64_t token)
{
	int ret = 0;
	char *full_path = NULL;
	
	if(pathname == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(pathname, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}
	
	FFILE *file_info = (FFILE *)calloc(1,sizeof(FFILE));
	if(file_info == NULL){
		DMCLOG_E("calloc file_info error");
		return FAIL;
	}

	//assert(file_info != NULL);
	file_info->token = token;
	file_info->offset = 0;
	file_info->srcPath = full_path;
	file_info->flag = flags;
	file_info->mode = mode;
	ret = handle_open_task(file_info);
	if(ret != 0){
		DMCLOG_E("handle open error");
		safe_free(file_info->srcPath);
		safe_free(file_info);
		return FAIL;
	}

	fd_info *open_fd_info = (fd_info *)calloc(1, sizeof(fd_info));
	if(open_fd_info == NULL){
		DMCLOG_E("malloc open_fd_info fail");
		safe_free(file_info->srcPath);
		safe_free(file_info);
		return FAIL;
	}

	open_fd_info->socket_fd = file_info->fd;
	open_fd_info->path = strdup(full_path);
	open_fd_info->offset = file_info->offset;
	open_fd_info->file_len = file_info->length;
	open_fd_info->mode = file_info->mode;
	open_fd_info->flag = file_info->flag;
	open_fd_info->type = fd_type_open;
	ret = add_info_for_ppc_fd_list(open_fd_info);
	if(ret){
		DMCLOG_E("add_info_for_ppc_fd_list fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info->srcPath);
		safe_free(file_info);
		return FAIL;
	}

	safe_free(file_info->srcPath);
	safe_free(file_info);
	return open_fd_info->v_fd;
}


/**
 * �ر��ļ�������
 */
int ppc_close(int fd)
{
	if(fd > 0){
		int socket_fd = get_socket_fd_from_ppc_fd_list(fd);
		if(socket_fd > 0){
			close(socket_fd);
			if(del_info_for_ppc_fd_list(fd) < 0)
				return FAIL;
			else
				return SUCCESS;
		}
		else{
			return FAIL;
		}
	}
	else{
		return FAIL;
	}
}

/*
 *ͨ���ļ���������ȡ�ļ�
 */
ssize_t ppc_read(int fd, void *buf, size_t count)
{
	int ret = 0;
	int socket_fd = get_socket_fd_from_ppc_fd_list(fd);
	if(socket_fd < 0){
		return FAIL;
	}

	ssize_t read_len = handle_read_task(socket_fd, count, buf);
	if(read_len > 0){
		ret = inc_offset_for_ppc_fd_list(fd, (off_t)read_len);
		if(ret){
			return FAIL;
		}
	}

	return read_len;
}

/*
 *ͨ���ļ�������д�ļ�
 */
ssize_t ppc_write(int fd, void *buf, size_t count)
{
	int ret = 0;
	int socket_fd = get_socket_fd_from_ppc_fd_list(fd);
	if(socket_fd < 0){
		return FAIL;
	}
	
	ssize_t write_len = handle_write_task(socket_fd, count, buf);
	if(write_len > 0){
		ret = inc_offset_for_ppc_fd_list(fd, (off_t)write_len);
		if(ret){
			return FAIL;
		}
	}

	return write_len;
}

/*
 *��ת��ָ���ļ�����λ��
 */
off_t ppc_lseek(int fd, off_t offset ,int whence, _int64_t token)
{
	int ret = 0;
	off_t seek_offset = 0;
	fd_info *open_fd_info = NULL;
	ret = get_info_from_ppc_fd_list(fd, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail");
		return FAIL;
	}
	DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);
	
	FFILE *file_info = (FFILE *)calloc(1,sizeof(FFILE));
	if(file_info == NULL){
		DMCLOG_E("calloc file_info error");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return FAIL;
	}

	if(whence == SEEK_SET){
		seek_offset = offset;
	}
	else if(whence == SEEK_CUR){
		seek_offset = open_fd_info->offset + offset;
	}
	else if(whence == SEEK_END){
		seek_offset = open_fd_info->file_len + offset;
	}
	else{
		DMCLOG_E("argument error");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	// TODO: openΪwriteʱ��file_lenΪ0
	if(!(open_fd_info->flag & (O_WRONLY | O_CREAT)) && (seek_offset > open_fd_info->file_len)){
		DMCLOG_E("seek_offset(%lld) is longer than file length(%lld)!!!", seek_offset, open_fd_info->file_len);
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	file_info->offset = seek_offset;
	file_info->srcPath = open_fd_info->path;
	file_info->flag = open_fd_info->flag;
	file_info->mode = open_fd_info->mode;
    file_info->token = token;
	ret = handle_open_task(file_info);
	if(ret != 0){
		DMCLOG_E("handle open error");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	//����ɵ�fd
	int socket_fd_old = open_fd_info->socket_fd;
	
	ret = ch_socket_fd_for_ppc_fd_list(fd, file_info->fd);
	if(ret){
		DMCLOG_E("change socket fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}
	//���ܵ���ppc_close
	close(socket_fd_old);

	ret = set_offset_for_ppc_fd_list(fd, file_info->offset);
	if(ret){
		DMCLOG_E("set_offset_for_ppc_fd_list fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	safe_free(open_fd_info->path);
	safe_free(open_fd_info);
	safe_free(file_info);

	return seek_offset;
}

/*
 *ͨ����ƫ������ԭ�Ӷ�ȡ�ļ�
 */
ssize_t ppc_pread(int fd, void *buf, size_t count, off_t offset, _int64_t token)
{
	int ret = 0;
	fd_info *open_fd_info = NULL;
	ret = get_info_from_ppc_fd_list(fd, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail");
		return FAIL;
	}
	DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);
	
	FFILE *file_info = (FFILE *)calloc(1,sizeof(FFILE));
	if(file_info == NULL){
		DMCLOG_E("calloc file_info error");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return FAIL;
	}

	if(offset > open_fd_info->file_len){
		DMCLOG_E("offset(%lld) is longer than file length(%lld)!!!", offset, open_fd_info->file_len);
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	file_info->offset = offset;
	file_info->srcPath = open_fd_info->path;
	file_info->flag = open_fd_info->flag;
	file_info->mode = open_fd_info->mode;
    file_info->token = token;
	ret = handle_open_task(file_info);
	if(ret != 0){
		DMCLOG_E("handle open error");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	//����ɵ�fd
	int socket_fd_old = open_fd_info->socket_fd;
	
	ret = ch_socket_fd_for_ppc_fd_list(fd, file_info->fd);
	if(ret){
		DMCLOG_E("change socket fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	//���ܵ���ppc_close
	close(socket_fd_old);

	ssize_t read_len = handle_read_task(file_info->fd, count, buf);
	if(read_len > 0){
		set_offset_for_ppc_fd_list(fd, read_len + offset);
	}

	safe_free(open_fd_info->path);
	safe_free(open_fd_info);
	safe_free(file_info);
	
	return read_len;
}

/*
 *ͨ����ƫ������ԭ��д�ļ�
 */
ssize_t ppc_pwrite(int fd, void *buf, size_t count, off_t offset, _int64_t token)
{
	int ret = 0;
	fd_info *open_fd_info = NULL;
	ret = get_info_from_ppc_fd_list(fd, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail");
		return FAIL;
	}
	DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);
	
	FFILE *file_info = (FFILE *)calloc(1,sizeof(FFILE));
	if(file_info == NULL){
		DMCLOG_E("calloc file_info error");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return FAIL;
	}

	#if 0
	if(offset > open_fd_info->file_len){
		DMCLOG_E("offset(%lld) is longer than file length(%lld)!!!", offset, open_fd_info->file_len);
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}
	#endif

	file_info->offset = offset;
	file_info->srcPath = open_fd_info->path;
	file_info->flag = open_fd_info->flag;
	file_info->mode = open_fd_info->mode;
    file_info->token = token;
	ret = handle_open_task(file_info);
	if(ret != 0){
		DMCLOG_E("handle open error");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	//����ɵ�fd
	int socket_fd_old = open_fd_info->socket_fd;
	
	ret = ch_socket_fd_for_ppc_fd_list(fd, file_info->fd);
	if(ret){
		DMCLOG_E("change socket fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return FAIL;
	}

	//���ܵ���ppc_close
	close(socket_fd_old);
	
	ssize_t write_len = handle_write_task(file_info->fd, count, buf);
	if(write_len > 0){
		set_offset_for_ppc_fd_list(fd, (off_t)write_len + offset);
	}

	safe_free(open_fd_info->path);
	safe_free(open_fd_info);
	safe_free(file_info);
	return write_len;
}

/*
  *�ۼ�д,���ռ��ڴ��з�ɢ�����ɻ������е�����д���ļ�������������
  */
ssize_t ppc_writev(int fd, const struct iovec *iov, int cnt, _int64_t token)
{
	int ret = 0;
	int num = 0;
	ssize_t write_len = 0;
	ssize_t write_all = 0;
	if(iov == NULL || cnt < 0){
		DMCLOG_D("invalid argument");
		return FAIL;
	}

	int socket_fd = get_socket_fd_from_ppc_fd_list(fd);
	if(socket_fd < 0){
		return FAIL;
	}
	
	for(num = 0; num < cnt; num++){	
		write_len = handle_write_task(socket_fd, iov[num].iov_len, (char *)(iov[num].iov_base));
		if(write_len > 0){
			ret = inc_offset_for_ppc_fd_list(fd, (off_t)write_len);
			if(ret){
				return FAIL;
			}
		}
		else{
			DMCLOG_E("handle_write_task fail");
			return FAIL;
		}
		write_all += write_len;
	}

	return write_all;
}

/*
  *����������
  */
int ppc_rename(const char *oldname, const char *newname, _int64_t token)
{
	int ret = 0;
	char *old_full_path = NULL;
	char *new_full_path = NULL;
	
	if(oldname == NULL || newname == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(oldname, &old_full_path, token);
	if(ret != 0 || old_full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}

	ret = create_new_full_path(newname, &new_full_path, token);
	if(ret != 0 || new_full_path == NULL){
		DMCLOG_D("create full path fail");
		safe_free(old_full_path);
		return FAIL;
	}
	
	ret = handle_rename_task(old_full_path, new_full_path, token);
	if(ret != 0){
		DMCLOG_E("handle_rename_task fail");
		safe_free(old_full_path);
		safe_free(new_full_path);
		return FAIL;
	}

	safe_free(old_full_path);
	safe_free(new_full_path);
	return SUCCESS;
}

/*
  *ɾ���ļ����Ӽ��ļ�
  */
int ppc_unlink(const char *pathname, _int64_t token)
{
	int ret = 0;
	char *full_path = NULL;

	if(pathname == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(pathname, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}

	ret = handle_unlink_task(full_path, token);
	if(ret != 0){
		DMCLOG_E("handle_unlink_task fail");
		safe_free(full_path);
		return FAIL;
	}

	safe_free(full_path);
	return SUCCESS;
}

/*
  function: �޸��ļ�����ʱ����޸�ʱ���ʱ���
  dirfd pathname:
  1)���pathname�����·�������������dirfd��·������������ڵ�ǰ���̹����ռ�;
  2)���pathname�����·������dirfdΪAT_FDCWD����������ڵ�ǰ���̹����ռ�;
  3)���pathname�Ǿ���·������dirfd�ᱻ����;
  flags:
  1)AT_SYMLINK_NOFOLLOW: ���pathָ��һ���������ӣ��򷵻ظ�������Ϣ;
  */
int ppc_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags, _int64_t token)
{
	char *real_path = NULL;
	struct timespec real_times[2];
	int ret = 0;
	fd_info *open_fd_info = NULL;

	if(pathname != NULL && *pathname != '/'){
		if(dirfd == AT_FDCWD){
			ret = create_new_full_path(pathname, &real_path, token);
			if(ret != 0 || real_path == NULL){
				DMCLOG_D("create real path fail");
				ret = FAIL;
				goto EXIT2;
			}	
		}
		else{
			ret = get_info_from_ppc_fd_list(dirfd, &open_fd_info);
			if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
				DMCLOG_E("get_info_from_ppc_fd_list fail");
				ret = FAIL;
				goto EXIT1;
			}
			DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);
			real_path = (char *)calloc(1, strlen(open_fd_info->path) + strlen(pathname) + 1);
			if(real_path == NULL){
				DMCLOG_E("mclloc fail");
				ret = FAIL;
				goto EXIT2;
			}
			memcpy(real_path, open_fd_info->path, strlen(open_fd_info->path));
			strcat(real_path, pathname);
		}
	}
	else if(pathname != NULL && *pathname == '/'){
		real_path = (char *)calloc(1, strlen(pathname) + 1);
		if(real_path == NULL){
			DMCLOG_E("mclloc fail");
			ret = FAIL;
			goto EXIT2;
		}
		memcpy(real_path, pathname, strlen(pathname));
	}
	else{
		DMCLOG_E("invalid argument");
		ret = FAIL;
		goto EXIT1;
	}
	DMCLOG_D("real_path: %s", real_path);

	if(times == NULL){
		struct timespec ts_now;
        ret = clock_gettime(CLOCK_REALTIME, &ts_now);
		DMCLOG_D("ret: %d, CLOCK_REALTIME: %lu, %lu\n", ret, ts_now.tv_sec, ts_now.tv_nsec);
		memcpy(&real_times[0], &ts_now, sizeof(struct timespec));
		memcpy(&real_times[1], &ts_now, sizeof(struct timespec));
	}
	else{
		if(times[0].tv_nsec == UTIME_NOW){
			struct timespec ts_now;
	        clock_gettime(CLOCK_REALTIME, &ts_now);
			memcpy(&real_times[0], &ts_now, sizeof(struct timespec));
		}
		#if 0
		else if(times[0].tv_nsec == UTIME_OMIT){
			real_times[0].tv_nsec = 0;
			real_times[0].tv_sec = 0;
		}
		#endif
		else{
			memcpy(&real_times[0], &times[0], sizeof(struct timespec));
		}

		if(times[1].tv_nsec == UTIME_NOW){
			struct timespec ts_now;
	        clock_gettime(CLOCK_REALTIME, &ts_now);
			memcpy(&real_times[1], &ts_now, sizeof(struct timespec));
		}
		#if 0
		else if(times[1].tv_nsec == UTIME_OMIT){
			real_times[1].tv_nsec = 0;
			real_times[1].tv_sec = 0;
		}
		#endif
		else{
			memcpy(&real_times[1], &times[1], sizeof(struct timespec));
		}
	}
	
	ret = handle_utimensat_task(real_path, real_times, flags, token);
	if(ret){
		DMCLOG_E("handle_utimensat_task fail");
		ret = FAIL;
		goto EXIT3;
	}
	ret = SUCCESS;

EXIT3:
	safe_free(real_path);
EXIT2:
	if(open_fd_info != NULL){
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
	}
EXIT1:
	return ret;	
}


/*
  *���ĵ�ǰ����Ŀ¼
  */
int ppc_chdir(const char *path, _int64_t token)
{
	int ret = 0;
	struct stat st;
	char *full_path = NULL;
	if(path == NULL){
		return FAIL;
	}				

	ret = create_new_full_path(path, &full_path, token);
	if(ret < 0){
		return FAIL;
	}
	else if(1 == ret){
		DMCLOG_D("no need change");
		return SUCCESS;
	}

	DMCLOG_D("full_path: %s", full_path);
	ret = handle_stat_task(full_path, &st, token);
	if(ret == 0 && st.st_mode == S_IFDIR){
		DMCLOG_D("check stat success");
	}
	else{
		DMCLOG_E("fial (ret %s, st.st_mode %d)", full_path, st.st_mode);
		safe_free(full_path);
		return FAIL;	
	}

	ret = update_work_dir_for_ppc_token_list(full_path, token);
	if(ret){
		DMCLOG_E("update work dir fail");
		safe_free(full_path);
		return FAIL;	
	}

	safe_free(full_path);
	return SUCCESS;	
}

/*
  *��ȡ��ǰ����Ŀ¼
  */
char *ppc_getwd(char *buf, _int64_t token)
{
	char *work_dir = NULL;
	if(buf == NULL){
		return NULL;
	}

	work_dir = get_work_dir_from_ppc_token_list(token);
	if(work_dir == NULL){
		return NULL;
	}
	else{
		memcpy(buf, work_dir, strlen(work_dir));
		safe_free(work_dir);
		return buf;
	}
}

/*
  *�ı��ļ�����
  */
int ppc_chmod(const char *pathname, mode_t mode, _int64_t token)
{
	return SUCCESS;
}

/*
  *�ı��ļ�����
  */
int ppc_fchmod(int filedes, mode_t mode, _int64_t token)
{
	return SUCCESS;
}

/*
  *�ı��ļ��û���
  */
int ppc_chown(const char *path, uid_t owner, gid_t group, _int64_t token)
{
	return SUCCESS;
}

/*
  *�ı��ļ��û���
  */
int ppc_fchown(int fd, uid_t owner, gid_t group, _int64_t token)
{
	return SUCCESS;
}

/*
  *�ı��ļ��û���
  */
int ppc_lchown(const char *path, uid_t owner, gid_t group, _int64_t token)
{
	return SUCCESS;
}

/*
  *��fdָ�����ļ���С��Ϊ����lengthָ���Ĵ�С
  */
int ppc_ftruncate(int fd, off_t length, _int64_t token)
{
	int ret = 0;
	fd_info *open_fd_info = NULL;
	ret = get_info_from_ppc_fd_list(fd, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail");
		return FAIL;
	}
	DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);

	ret = handle_ftruncate_task(open_fd_info->path, length, token);
	if(ret){
		DMCLOG_E("handle_ftruncate_task fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return FAIL;
	}
	else{
		DMCLOG_D("handle_ftruncate_task success");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return SUCCESS;
	}
}

/*
  *Ϊ�ļ�Ԥ��������ռ�
  */
int ppc_fallocate(int fd, int mode, off_t offset, off_t len, _int64_t token)
{
	int ret = 0;
	fd_info *open_fd_info = NULL;
	ret = get_info_from_ppc_fd_list(fd, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail");
		return FAIL;
	}
	DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);

	ret = handle_fallocate_task(open_fd_info->path, mode, offset, len, token);
	if(ret){
		DMCLOG_E("handle_fallocate_task fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return FAIL;
	}
	else{
		DMCLOG_D("handle_fallocate_task success");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		return SUCCESS;
	}
}

/*
  *ʵ���㿽��
  */
ssize_t ppc_sendfile(int out_fd, int in_fd, off_t *offset, size_t count, _int64_t token)
{
	return SUCCESS;
}

/*
  *ʵ���㿽��
  */
ssize_t ppc_splice(int fd_in,loff_t* off_t,int fd_out,loff_t* off_out,size_t len,unsigned int flags, _int64_t token)
{
	return SUCCESS;
}

/*
  *�ļ�ͬ��
  */
int ppc_fsync(int fd, _int64_t token)
{
	return SUCCESS;
}

/*
  *������������
  */
int ppc_symlink(const char *oldpath, const char *newpath, _int64_t token)
{
	int ret = 0;
	char *old_full_path = NULL;
	char *new_full_path = NULL;
	
	if(oldpath == NULL || newpath == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(oldpath, &old_full_path, token);
	if(ret != 0 || old_full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}

	ret = create_new_full_path(newpath, &new_full_path, token);
	if(ret != 0 || new_full_path == NULL){
		DMCLOG_D("create full path fail");
		safe_free(old_full_path);
		return FAIL;
	}
	
	ret = handle_symlink_task(old_full_path, new_full_path, token);
	if(ret != 0){
		DMCLOG_E("ppc_symlink fail");
		safe_free(old_full_path);
		safe_free(new_full_path);
		return FAIL;
	}

	safe_free(old_full_path);
	safe_free(new_full_path);
	return SUCCESS;
}

/*
  *ȡ�÷���������ָ���ļ�
  */
ssize_t ppc_readlink(const char *path, char *buf, size_t bufsiz, _int64_t token)
{
	int ret = 0;
	char *link_buf = NULL;
	char *full_path = NULL;
	ssize_t link_len = 0;

	if(path == NULL || buf == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(path, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}
	
	ret = handle_readlink_task(full_path, &link_buf, token);
	if(ret != 0 || link_buf == NULL){
		DMCLOG_E("ppc_readlink fail");
		safe_free(full_path);
		return FAIL;
	}

	link_len = strlen(link_buf);
	if(link_len > bufsiz){
		DMCLOG_E("link_buf() is longer than bufsiz(%d)", strlen(link_buf), bufsiz);
		safe_free(full_path);
		safe_free(link_buf);
	}
	
	memcpy(buf, link_buf, link_len);

	safe_free(full_path);
	safe_free(link_buf);
	
	return link_len;
}


/*
  *����Ӳ����
  */
int ppc_link (const char * oldpath, const char *newpath, _int64_t token)
{
	int ret = 0;
	char *old_full_path = NULL;
	char *new_full_path = NULL;
	
	if(oldpath == NULL || newpath == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(oldpath, &old_full_path, token);
	if(ret != 0 || old_full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}

	ret = create_new_full_path(newpath, &new_full_path, token);
	if(ret != 0 || new_full_path == NULL){
		DMCLOG_D("create full path fail");
		safe_free(old_full_path);
		return FAIL;
	}
	
	ret = handle_link_task(old_full_path, new_full_path, token);
	if(ret != 0){
		DMCLOG_E("handle_link_task fail");
		safe_free(old_full_path);
		safe_free(new_full_path);
		return FAIL;
	}

	safe_free(old_full_path);
	safe_free(new_full_path);
	return SUCCESS;
}

/*
  *�����ļ�
  */
int ppc_mknod(const char *path, mode_t mode, dev_t dev, _int64_t token)
{
	return SUCCESS;
}

/*
  *��ȡ�ļ�����·��
  */
char *ppc_realpath(const char *path, char *resolved_path, _int64_t token)
{
	int ret = 0;
	char *full_path = NULL;
	
	if(path == NULL || resolved_path == NULL){
		DMCLOG_E("para is null");
		return NULL;
	}

	ret = create_new_full_path(path, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return NULL;
	}

	memcpy(resolved_path, full_path, strlen(full_path));
	safe_free(full_path);
	return resolved_path;
}

/*
  *��ȡ�ļ�ϵͳ��Ϣ
  */
int ppc_statvfs(const char *path, struct statvfs *buf, _int64_t token)
{
	int ret = 0;
	char *full_path = NULL;
	
	if(path == NULL || buf == NULL){
		DMCLOG_E("para is null");
		return FAIL;
	}

	ret = create_new_full_path(path, &full_path, token);
	if(ret != 0 || full_path == NULL){
		DMCLOG_D("create full path fail");
		return FAIL;
	}
	DMCLOG_D("full_path: %s", full_path);

	ret = handle_statvfs_task(full_path, buf, token);
	if(ret != 0){
		DMCLOG_E("handle_link_task fail");
		safe_free(full_path);
		return FAIL;
	}
	
	return SUCCESS;
}

