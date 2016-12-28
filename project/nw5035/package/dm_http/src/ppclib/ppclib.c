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
	int ret = handle_open_task(fp);
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
	 return handle_read_task(fp,count,buffer);
}

/**
 * �ļ�д����
 */
size_t ppc_fwrite(const void* buffer, size_t size, size_t count, PFILE* fp)
{
	return handle_write_task(fp,count,buffer);
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
	handle_close_task(fp);
	fp->offset = fromwhere + offset;
    int ret = handle_open_task(fp);
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
	handle_close_task(fp);
	safe_free(fp);
}

PPC_DIR *ppc_opendir(const char *path,char *token)
{
    PPC_DIR * p_dir = (PPC_DIR *)calloc(1,sizeof(PPC_DIR));
    int ret = handle_opendir_task(path,p_dir,token);
    if(ret != 0)
    {
        return NULL;
    }
    return p_dir;
}

struct ppc_dirent *ppc_readdir(PPC_DIR *p_dir,char *token)
{
    return handle_readdir_task(p_dir);
}

void ppc_rewinddir(PPC_DIR *dp,char *token)
{

}

int ppc_closedir(PPC_DIR *dp,char *token)
{
    return handle_closedir_task(dp);
}

long ppc_telldir(PPC_DIR *dp,char *token)
{
    return handle_telldir_task(dp);
}

void ppc_seekdir(PPC_DIR *dp,long loc,char *token)
{
    handle_seekdir_task(dp, loc);
}

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



