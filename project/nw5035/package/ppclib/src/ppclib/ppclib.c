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
#include "ppc_list.h"

/**
  * function:��ȡ��汾��Ϣ
  * ppc_version: ��汾��Ϣ
  * return:0 sucess,!0 failed
  */
int ppc_get_pkg_version(pkg_version *pkg_version_info)
{
	if(pkg_version_info == NULL){
		return -1;
	}

	strcpy(pkg_version_info->pkg_name, PKG_NAME);
	strcpy(pkg_version_info->pkg_version, PKG_VERSION);

	return 0;
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
ERROR_CODE_PPC ppc_login(char* username,char* password,char **token)
{
    int res = handle_login_task(username,password,token);
    if(res != 0)
    {
        return USERNAME_NOT_FOUND;
    }
    
	init_ppc_fd_list();
	return SUCCESS;
}
/**
 * function:�û��˳�
 * param:
 * token �û���ʶ
 * return:0 sucess,!0 failed
 */
ERROR_CODE_PPC ppc_logout(char *token)
{
    int res = handle_logout_task(token);
    if(res != 0)
    {
        return USERNAME_NOT_FOUND;
    }
	free_ppc_fd_list();
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
PFILE* ppc_fopen(const char *path,const char *mode,char *token)
{
	if(path == NULL)
	{
		DMCLOG_E("para is null");
		return NULL;
	}
	PFILE *fp = (PFILE *)calloc(1,sizeof(PFILE));
	assert(fp != NULL);
    fp->token = token;
	strcpy(fp->mode,mode);
	fp->offset = 0;
	fp->srcPath = path;
    //fp->token = (char *)token;
	int ret = handle_fopen_task(fp);
	if(ret != 0)
	{
		DMCLOG_E("handle open error");
		safe_free(fp);
		return NULL;
	}

	return fp;
}

/**
 * �ļ���ȡ����
 */
size_t ppc_fread( void *buffer, size_t size, size_t count, PFILE *fp)
{
	 return handle_fread_task(fp,count,buffer);
}

/**
 * �ļ�д����
 */
size_t ppc_fwrite(const void* buffer, size_t size, size_t count, PFILE* fp)
{
	return handle_fwrite_task(fp,count,buffer);
}

/**
 * �ļ�ָ��ƫ��
 * fromwhere��ƫ����ʼλ�ã��ļ�ͷ0(SEEK_SET)����ǰλ��1(SEEK_CUR)���ļ�β2(SEEK_END)��Ϊ��׼��ƫ��offset��ָ��ƫ���������ֽڵ�λ�á�
 */
off_t ppc_fseek( PFILE* fp, off_t offset, int fromwhere)
{
	if(fp == NULL||fp->srcPath == NULL)
	{
		DMCLOG_E("para is null");
		return -1;
	}
	handle_fclose_task(fp);
	fp->offset = fromwhere + offset;
    int ret = handle_fopen_task(fp);
    if(ret != 0)
    {
        DMCLOG_E("handle open error");
        safe_free(fp);
        return -1;
    }
	return fp->offset;
}


/**
 * �ر��ļ�ָ��
 */
void ppc_fclose( PFILE *fp)
{
	handle_fclose_task(fp);
	safe_free(fp);
}

/*
 *��Ŀ¼
 */
PPC_DIR *ppc_opendir(const char *path,char *token)
{
    PPC_DIR * p_dir = (PPC_DIR *)calloc(1,sizeof(PPC_DIR));
    int ret = handle_opendir_task(path,p_dir,token);
    if(ret != 0)
    {
    	safe_free(p_dir);
        return NULL;
    }
    return p_dir;
}

/*
 *��Ŀ¼
 */
PPC_DIR *ppc_fdopendir(int fd, char *token)
{
	int ret = 0;
	fd_info *open_fd_info = NULL;

	ret = get_info_from_ppc_fd_list(fd, &open_fd_info);
	if(ret || (open_fd_info == NULL) || (open_fd_info->path == NULL)){
		DMCLOG_E("get_info_from_ppc_fd_list fail");
		return NULL;
	}
	DMCLOG_D("open_fd_info->path: %s", open_fd_info->path);

	PPC_DIR * p_dir = (PPC_DIR *)calloc(1,sizeof(PPC_DIR));
	if(p_dir == NULL){
		DMCLOG_E("p_dir calloc,fail");
		return NULL;
	}


	//ret = handle_fdopendir_task(fd, p_dir, token);
	ret = handle_opendir_task(open_fd_info->path, p_dir, token);
	if(ret != 0){
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
    	safe_free(p_dir);
        return NULL;
    }
	safe_free(open_fd_info->path);
	safe_free(open_fd_info);
	return p_dir;
}

/*
 *��ȡĿ¼
 */
struct ppc_dirent *ppc_readdir(PPC_DIR *p_dir,char *token)
{
    return handle_readdir_task(p_dir);
}

/*
 *����Ŀ¼ָ��
 */
void ppc_rewinddir(PPC_DIR *dp,char *token)
{
	handle_rewinddir_task(dp);
	return ;
}

/*
 *�ر�Ŀ¼
 */
int ppc_closedir(PPC_DIR *dp,char *token)
{
    return handle_closedir_task(dp);
}

/*
 *��ȡĿ¼λ��
 */
off_t ppc_telldir(PPC_DIR *dp,char *token)
{
    return handle_telldir_task(dp);
}

/*
 *����ָ��Ŀ¼λ��
 */
void ppc_seekdir(PPC_DIR *dp,off_t loc,char *token)
{
	handle_seekdir_task(dp, loc);
    return;
}

/*
 *����Ŀ¼
 */
int ppc_mkdir(const char *dirname, mode_t mode, char *token)
{
	// TODO: ȱ��mode����
	return handle_mkdir_task(dirname, mode, token);
}

/*
 *ɾ��Ŀ¼
 */
int ppc_rmdir(const char *pathname, char *token)
{	
	//�ļ��������ļ�ʱ��rmdirӦ����ʧ��
	return handle_rmdir_task(pathname, token);
}

/*
 *��ȡ�ļ���Ϣ
 */
int ppc_stat(const char *file_name, struct ppc_stat *buf,char *token)
{
    int ret = handle_stat_task(file_name,buf,token);
    if(ret != 0)
    {
        DMCLOG_E("stat %s error",file_name);
        return ret;
    }
    return ret;
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
int ppc_open(const char*pathname, int flags, mode_t mode, char *token)
{
	ENTER_FUNC();
	if(pathname == NULL)
	{
		DMCLOG_E("para is null");
		return NULL;
	}
	FFILE *file_info = (FFILE *)calloc(1,sizeof(FFILE));
	if(file_info == NULL){
		DMCLOG_E("calloc file_info error");
		return -1;
	}

	//assert(file_info != NULL);
	file_info->token = token;
	file_info->offset = 0;
	file_info->srcPath = pathname;
	file_info->flag = flags;
	file_info->mode = mode;
    //file_info->token = (char *)token;
	int ret = handle_open_task(file_info);
	if(ret != 0){
		DMCLOG_E("handle open error");
		safe_free(file_info);
		return -1;
	}

	fd_info *open_fd_info = (fd_info *)calloc(1, sizeof(fd_info));
	if(open_fd_info == NULL)
	{
		DMCLOG_E("malloc open_fd_info fail");
		safe_free(file_info);
		return -1;
	}
	
	open_fd_info->fd = file_info->fd;
	open_fd_info->socket_fd = file_info->fd;
	open_fd_info->path = strdup(pathname);
	ret = add_info_for_ppc_fd_list(open_fd_info);
	if(ret){
		DMCLOG_E("add_info_for_ppc_fd_list fail");
		safe_free(open_fd_info->path);
		safe_free(open_fd_info);
		safe_free(file_info);
		return -1;
	}

	return file_info->fd;
}


/**
 * �ر��ļ�������
 */
int ppc_close(int fd)
{
	if(fd){
		del_info_for_ppc_fd_list(fd);
		return close(fd);
	}
	return 0;
}

/*
 *ͨ���ļ���������ȡ�ļ�
 */
ssize_t ppc_read(int fd, void *buf, size_t count)
{
	return handle_read_task(fd, count, buf);
}

/*
 *ͨ���ļ�������д�ļ�
 */
ssize_t ppc_write(int fd, void *buf, size_t count)
{
	return handle_write_task(fd, count, buf);
}


