#include "db_opr.h"
#include "token_manage.h"
#include "bfavfs.h"
#include "base.h"
//#include "media_process.h"



static int get_bucket_name(char *path,char *bucket_name)
{
	char *tmp = strchr(path,'/');
	if(tmp != NULL)
	{
		*tmp = 0;
		strcpy(bucket_name,path);
		*tmp = '/';
		DMCLOG_D("bucket_name = %s",bucket_name);
	}else{
		strcpy(bucket_name,path);
	}
	return 0;
}


BucketObject *build_bucket_object(char *path,void *token)
{
	if(token == NULL)
	{
		DMCLOG_E("para is null");
		return -1;
	}
	token_dnode_t *token_dnode = (token_dnode_t *)token;
	BucketObject *bObject = (BucketObject *)calloc(1,sizeof(BucketObject));
	assert(bObject != NULL);
	int res = get_bucket_name(path,bObject->bucket_name);
	if(res != 0)
	{
		DMCLOG_E("get bucket name from path error");
		return NULL;
	}

	if(token_dnode->isPublicUser == false)//cur user is nornal user
	{
		if(strcmp(bObject->bucket_name,PUBLIC_PATH))//it is not public path
		{
			S_STRNCPY(bObject->bucket_name,token_dnode->bucket_name,MAX_BUCKET_NAME_LEN);
			if(*path)
			{
				sprintf(bObject->path,"/%s/%s",bObject->bucket_name,path);
			}else{
				sprintf(bObject->path,"/%s",bObject->bucket_name);
			}
			token_dnode->isPublicPath = false;
		}else{
			strcpy(bObject->bucket_name,PUBLIC_BUCKET_NAME);
			if(*(path + strlen(PUBLIC_PATH) + 1))
			{
				sprintf(bObject->path,"/%s/%s",bObject->bucket_name,path + strlen(PUBLIC_PATH) + 1);
			}else{
				sprintf(bObject->path,"/%s",bObject->bucket_name);
			}
			
			DMCLOG_D("bObject->path = %s",bObject->path);
			token_dnode->isPublicPath = true;
		}
	}else{
		S_STRNCPY(bObject->bucket_name,token_dnode->bucket_name,MAX_BUCKET_NAME_LEN);
		if(*path)
		{
			sprintf(bObject->path,"/%s/%s",bObject->bucket_name,path);
		}else{
			sprintf(bObject->path,"/%s",bObject->bucket_name);
		}
		token_dnode->isPublicPath = false;
	}
	return bObject;
}

ERROR_CODE_VFS _bfavfs_get_file_list(BucketObject* sObject,v_file_list_t *vlist,void * token)
{
	ENTER_FUNC();
	v_file_query_t v_file_query;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	
	v_file_query.cmd = V_FILE_TABLE_QUERY_LIST;
	if(sObject->file_type == 0)
	{
		S_STRNCPY(vlist->path,sObject->path,MAX_FILE_PATH_LEN);
		S_STRNCPY(vlist->bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
		S_STRNCPY(v_file_query.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
	}else{
		token_dnode_t *token_dnode = (token_dnode_t *)token;
		S_STRNCPY(vlist->bucket_name,token_dnode->bucket_name,MAX_BUCKET_NAME_LEN);
		S_STRNCPY(v_file_query.bucket_name,token_dnode->bucket_name,MAX_BUCKET_NAME_LEN);
	}
		
	memcpy(&v_file_query.v_file_list,vlist,sizeof(v_file_list_t));
	int res = _handle_v_file_table_query(&v_file_query);
	if(res != 0)
	{
		DMCLOG_E("query v file list table error");
		EXIT_FUNC();
		return -1;
	}
	vlist->total = v_file_query.v_file_list.total;
	DMCLOG_D("vlist->total = %d",vlist->total);
	EXIT_FUNC();
	return 0;
}




VFILE* _bfavfs_fopen(BucketObject* sObject,const char * mode,void * token)
{
	int res = 0;
	char real_path[32] = {0};
	v_file_query_t v_file_query;
	int flags;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
	if(strstr(mode,"r"))//download
	{
		flags = O_RDONLY;
		res = _handle_v_file_table_query(&v_file_query);
		if(res != 0)
		{
			DMCLOG_E("query file table error");
			return NULL;
		}
		DMCLOG_D("real_path = %s",v_file_query.v_file_info.real_path);
		S_STRNCPY(real_path,v_file_query.v_file_info.real_path,MAX_REAL_PATH_LEN);
	}else if(strstr(mode,"w")||strstr(mode,"a"))//upload
	{
		flags = O_WRONLY | O_CREAT;
		res = _handle_v_file_table_query(&v_file_query);
		if(res != 0)
		{
			// 1 :get new path
			res =  rfsvfs_get_new_file_path(real_path);
			if(res != 0)
			{
				DMCLOG_E("get new file path error");
				return NULL;
			}
			if(sObject->offset > 0)
			{
				//truncate(real_path,sObject->offset);
			}
			// 2 :insert new path to file table
			v_file_insert_t v_file_insert;
			memset(&v_file_insert,0,sizeof(v_file_insert_t));
			v_file_insert.cmd = V_FILE_TABLE_INSERT_INFO;
			S_STRNCPY(v_file_insert.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
			DMCLOG_D("real path:%s,bucket_name = %s",real_path,sObject->bucket_name);
			S_STRNCPY(v_file_insert.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
			S_STRNCPY(v_file_insert.v_file_info.real_path,real_path,MAX_REAL_PATH_LEN);
			DMCLOG_D("path = %s,real_path = %s",v_file_insert.v_file_info.path,real_path);
			res = _handle_v_file_table_insert(&v_file_insert);
			if(res != 0)
			{
				DMCLOG_E("insert file table error");
				return NULL;
			}
		}else{
			S_STRNCPY(real_path,v_file_query.v_file_info.real_path,MAX_REAL_PATH_LEN);
		}
	}

	VFILE *vf = (VFILE *)calloc(1,sizeof(VFILE));
	assert(vf != NULL);
#ifdef RFSVFS_STREAM
	FILE *pf = rfsvfs_fopen(real_path, mode);
	if(pf == NULL)
	{
		DMCLOG_E("fopen %s error",real_path);
		return NULL;
	}
	
	vf->fp = pf;
#else
	int fd = rfsvfs_open(real_path,flags,0644);
	vf->fd = fd;
#endif
	
	vf->srcPath = (char *)calloc(1,strlen(sObject->path) + 1);
	assert(vf->srcPath != NULL);
	strcpy(vf->srcPath,sObject->path);

	vf->realPath = (char *)calloc(1,strlen(real_path) + 1);
	assert(vf->realPath != NULL);
	strcpy(vf->realPath,real_path);

	vf->bobject = (BucketObject *)calloc(1,sizeof(BucketObject));
	assert(vf->bobject != NULL);

	strcpy(vf->bobject,sObject);
	return vf;
}

VFILE* bfavfs_fopen(BucketObject* sObject,const char * mode,void * token)
{
	int res = 0;
	char real_path[32] = {0};
	v_file_query_t v_file_query;
	int flags;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
	if(strstr(mode,"r"))//download
	{
		flags = O_RDONLY;
		res = _handle_v_file_table_query(&v_file_query);
		if(res != 0)
		{
			DMCLOG_E("query file table error");
			return NULL;
		}
		DMCLOG_D("real_path = %s",v_file_query.v_file_info.real_path);
		S_STRNCPY(real_path,v_file_query.v_file_info.real_path,MAX_REAL_PATH_LEN);
	}else if(strstr(mode,"w")||strstr(mode,"a"))//upload
	{
		flags = O_WRONLY | O_CREAT;
		res = _handle_v_file_table_query(&v_file_query);
		if(res != 0)
		{
			// 1 :get new path
			res =  rfsvfs_get_new_file_path(real_path);
			if(res != 0)
			{
				DMCLOG_E("get new file path error");
				return NULL;
			}
			if(sObject->offset > 0)
			{
				//truncate(real_path,sObject->offset);
			}
			// 2 :insert new path to file table
			v_file_insert_t v_file_insert;
			memset(&v_file_insert,0,sizeof(v_file_insert_t));
			v_file_insert.cmd = V_FILE_TABLE_INSERT_INFO;
			S_STRNCPY(v_file_insert.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
			DMCLOG_D("real path:%s,bucket_name = %s",real_path,sObject->bucket_name);
			S_STRNCPY(v_file_insert.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
			S_STRNCPY(v_file_insert.v_file_info.real_path,real_path,MAX_REAL_PATH_LEN);
			DMCLOG_D("path = %s,real_path = %s",v_file_insert.v_file_info.path,real_path);
			res = _handle_v_file_table_insert(&v_file_insert);
			if(res != 0)
			{
				DMCLOG_E("insert file table error");
				return NULL;
			}
		}else{
			S_STRNCPY(real_path,v_file_query.v_file_info.real_path,MAX_REAL_PATH_LEN);
		}
	}

	VFILE *vf = (VFILE *)calloc(1,sizeof(VFILE));
	assert(vf != NULL);
	FILE *pf = rfsvfs_fopen(real_path, mode);
	if(pf == NULL)
	{
		DMCLOG_E("fopen %s error",real_path);
		return NULL;
	}
	
	vf->fp = pf;
	
	vf->srcPath = (char *)calloc(1,strlen(sObject->path) + 1);
	assert(vf->srcPath != NULL);
	strcpy(vf->srcPath,sObject->path);

	vf->realPath = (char *)calloc(1,strlen(real_path) + 1);
	assert(vf->realPath != NULL);
	strcpy(vf->realPath,real_path);

	vf->bobject = (BucketObject *)calloc(1,sizeof(BucketObject));
	assert(vf->bobject != NULL);

	strcpy(vf->bobject,sObject);
	return vf;
}


/**
 * �ļ���ȡ����
 */
size_t _bfavfs_fread( void *buffer, size_t size, size_t count, VFILE *vf)
{
#ifdef RFSVFS_STREAM
	return rfsvfs_fread(buffer,size,count,vf->fp);
#else
	return rfsvfs_read(vf->fd, buffer, count);
#endif	
}


/**
 * �ļ�д����
 */
size_t _bfavfs_fwrite(const void* buffer, size_t size, size_t count, VFILE* vf)
{
#ifdef RFSVFS_STREAM
	return rfsvfs_fwrite( buffer, size,count, vf->fp);
#else
	return rfsvfs_write( vf->fd,buffer,count);
#endif
}


/**
 * �ļ�ָ��ƫ��
 * fromwhere��ƫ����ʼλ�ã��ļ�ͷ0(SEEK_SET)����ǰλ��1(SEEK_CUR)���ļ�β2(SEEK_END)��Ϊ��׼��ƫ��offset��ָ��ƫ���������ֽڵ�λ�á�
 */
int _bfavfs_fseek(VFILE *vf, long offset, int fromwhere)
{
#ifdef RFSVFS_STREAM
	return rfsvfs_fseek(vf->fp,offset,fromwhere);
#else
	return rfsvfs_lseek(vf->fd,offset,fromwhere);
#endif
}


/**
 * �ļ���ȡ����
 */
size_t bfavfs_fread( void *buffer, size_t size, size_t count, VFILE *vf)
{
	return rfsvfs_fread(buffer,size,count,vf->fp);
}


/**
 * �ļ�д����
 */
size_t bfavfs_fwrite(const void* buffer, size_t size, size_t count, VFILE* vf)
{
	return rfsvfs_fwrite( buffer, size,count, vf->fp);
}


/**
 * �ļ�ָ��ƫ��
 * fromwhere��ƫ����ʼλ�ã��ļ�ͷ0(SEEK_SET)����ǰλ��1(SEEK_CUR)���ļ�β2(SEEK_END)��Ϊ��׼��ƫ��offset��ָ��ƫ���������ֽڵ�λ�á�
 */
int bfavfs_fseek(VFILE *vf, long offset, int fromwhere)
{
	return rfsvfs_fseek(vf->fp,offset,fromwhere);
}

/**
 * �ж��ļ��Ƿ��Ѷ���
 * �ļ�δ��������0
 * �ļ���������1
 */
int bfavfs_feof(VFILE* vf)
{
	return rfsvfs_feof(vf->fp);
}

/**
 * �ر��ļ�ָ��
 */
int _bfavfs_fclose( VFILE *vf,void* token)
{
	int res = 0;
#ifdef RFSVFS_STREAM
	rfsvfs_fclose(vf->fp);
#else
	rfsvfs_close(vf->fd);
#endif
	struct stat st;
	if(rfsvfs_fstat(vf->realPath,&st) != 0)
	{
		DMCLOG_E("stat error");
		res = -1;
		goto EXIT;
	}
	
	v_file_update_t v_file_update;
	memset(&v_file_update,0,sizeof(v_file_update_t));
	S_STRNCPY(v_file_update.v_file_info.path,vf->srcPath,MAX_FILE_PATH_LEN);
	DMCLOG_D("vf->srcPath = %s",vf->srcPath);
	S_STRNCPY(v_file_update.bucket_name,vf->bobject->bucket_name,MAX_BUCKET_NAME_LEN);
	DMCLOG_D("bucket_name = %s",v_file_update.bucket_name);
	
	v_file_update.v_file_info.atime = st.st_atime;
	v_file_update.v_file_info.ctime = st.st_ctime;
	v_file_update.v_file_info.mtime = st.st_mtime;
	v_file_update.v_file_info.size = st.st_size;
	DMCLOG_D("st.st_size = %lld,v_file_update.v_file_info.size = %lld",(long long)st.st_size,(long long)v_file_update.v_file_info.size);
	v_file_update.v_file_info.isDir = S_ISDIR(st.st_mode);
	v_file_update.v_file_info.type =  db_get_mime_type(vf->srcPath,strlen(vf->srcPath));
	if(*vf->uuid)
	{
		S_STRNCPY(v_file_update.v_file_info.uuid,vf->uuid,MAX_FILE_UUID_LEN);
	}else{
		strcpy(v_file_update.v_file_info.uuid,"13141314");
	}
	v_file_update.cmd = V_FILE_TABLE_UPDATE_FILE_INFO;
	v_file_update.remove = rfsvfs_remove;
	res = _handle_v_file_table_update(&v_file_update);
	if(res != 0)
	{
		DMCLOG_E("update file table error");
		res = -1;
		goto EXIT;
	}
EXIT:
	
	safe_free(vf->srcPath);
	safe_free(vf->realPath);
	safe_free(vf);
	return res;
}


/**
 * �ر��ļ�ָ��
 */
int bfavfs_fclose( VFILE *vf,void* token)
{
	int res = 0;
	rfsvfs_fclose(vf->fp);
	struct stat st;
	if(rfsvfs_fstat(vf->realPath,&st) != 0)
	{
		DMCLOG_E("stat error");
		res = -1;
		goto EXIT;
	}
	
	v_file_update_t v_file_update;
	memset(&v_file_update,0,sizeof(v_file_update_t));
	S_STRNCPY(v_file_update.v_file_info.path,vf->srcPath,MAX_FILE_PATH_LEN);
	S_STRNCPY(v_file_update.bucket_name,vf->bobject->bucket_name,MAX_BUCKET_NAME_LEN);
	
	v_file_update.v_file_info.atime = st.st_atime;
	v_file_update.v_file_info.ctime = st.st_ctime;
	v_file_update.v_file_info.mtime = st.st_mtime;
	v_file_update.v_file_info.size = (long long)st.st_size;
	DMCLOG_D("st.st_size = %lld,v_file_update.v_file_info.size = %lld",(long long)st.st_size,(long long)v_file_update.v_file_info.size);
	v_file_update.v_file_info.isDir = S_ISDIR(st.st_mode);
	v_file_update.v_file_info.type =  db_get_mime_type(vf->srcPath,strlen(vf->srcPath));

	if(*vf->uuid)
	{
		S_STRNCPY(v_file_update.v_file_info.uuid,vf->uuid,MAX_FILE_UUID_LEN);
	}else{
		strcpy(v_file_update.v_file_info.uuid,"13141314");
	}
	v_file_update.cmd = V_FILE_TABLE_UPDATE_FILE_INFO;
	v_file_update.remove = rfsvfs_remove;
	res = _handle_v_file_table_update(&v_file_update);
	if(res != 0)
	{
		DMCLOG_E("update file table error");
		res = -1;
		goto EXIT;
	}
EXIT:
	
	safe_free(vf->srcPath);
	safe_free(vf->realPath);
	safe_free(vf);
	return res;
}

/**
 * ��ȡ�ļ�����
 */
int bfavfs_fstat(const char *path,struct stat *st,void *token)
{
	ENTER_FUNC();
	token_dnode_t *token_dnode = (token_dnode_t *)token;
	v_file_info_t v_file_info;
	memset(&v_file_info,0,sizeof(v_file_info_t));
	char db_path[MAX_FILE_PATH_LEN] = {0};
	if(*path)
	{
		sprintf(db_path,"/%s/%s",token_dnode->bucket_name,path);
	}else{
		sprintf(db_path,"/%s",token_dnode->bucket_name);
	}
	S_STRNCPY(v_file_info.path,db_path,MAX_FILE_PATH_LEN);
	int res = handle_v_file_table_query(token_dnode->bucket_name,&v_file_info);
	if(res != 0)
	{
		DMCLOG_E("query v file table error");
		return -1;
	}
	
	if(*v_file_info.real_path)
	{
		DMCLOG_D("real_path:%s",v_file_info.real_path);
		res = rfsvfs_fstat(v_file_info.real_path,st);
		if(res != 0)
		{
			DMCLOG_E("stat %s error[%d]",path,errno);
			return -1;
		}
	}else{
		st->st_size = 0;
		st->st_mode = S_IFDIR;
	}
	
	EXIT_FUNC();
	return 0;
}

/**
 * ��ȡ�ļ�����
 */
int _bfavfs_fstat(BucketObject* sObject,struct stat *st,void *token)
{
	v_file_query_t v_file_query;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
	int res = _handle_v_file_table_query(&v_file_query);
	if(res != 0)
	{
		DMCLOG_E("query v file table error");
		return -1;
	}
	
	if(*v_file_query.v_file_info.real_path)
	{
		DMCLOG_D("real_path:%s",v_file_query.v_file_info.real_path);
		res = rfsvfs_fstat(v_file_query.v_file_info.real_path,st);
		if(res != 0)
		{
			DMCLOG_E("stat %s error[%d]",v_file_query.v_file_info,errno);
			return -1;
		}
	}else{
		st->st_mode = S_IFDIR;
		st->st_size = 4096;
		st->st_nlink = 2;
		st->st_ino = v_file_query.v_file_info.id;
		st->st_dev = 0;
		st->st_rdev = 0;
		st->st_gid = 0;
		st->st_uid = 0;
		st->st_blksize = 4096;
		st->st_blocks = 8;
		st->st_atime = v_file_query.v_file_info.atime;
		st->st_mtime = v_file_query.v_file_info.mtime;
		st->st_ctime = v_file_query.v_file_info.ctime;
	}
	return 0;
}


/**
 *ͨ��UUID �ж��ļ��Ƿ����
 */
int bfavfs_exist(const char *uuid,void *token)
{
	ENTER_FUNC();
	token_dnode_t *token_dnode = (token_dnode_t *)token;

	int res = handle_file_uuid_exist_query(token_dnode->bucket_name,uuid);
	if(res != 0)
	{
		DMCLOG_E("query v file table by uuid error");
		return -1;
	}
	return 0;
}


/**
 *ͨ��UUID list �ж��ļ��Ƿ����
 */
int bfavfs_list_exist(struct dl_list *head,void *token)
{
	ENTER_FUNC();
	token_dnode_t *token_dnode = (token_dnode_t *)token;

	int res = handle_uuid_list_exist_query(token_dnode->bucket_name,head);
	if(res != 0)
	{
		DMCLOG_E("query v file table by uuid error");
		return -1;
	}
	return 0;
}

/**
 *ͨ��UUID list �ж��ļ��Ƿ����
 */
int _bfavfs_exist(BucketObject* sObject,void *token)
{
	v_file_query_t v_file_query;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	token_dnode_t *token_dnode = (token_dnode_t *)token;
	
	S_STRNCPY(v_file_query.bucket_name,token_dnode->bucket_name,MAX_BUCKET_NAME_LEN);
	if(sObject->head != NULL)
	{
		memcpy(&v_file_query.v_file_list.head,sObject->head,sizeof(struct dl_list));
		v_file_query.cmd = V_FILE_TABLE_QUERY_LIST_BY_UUID;
	}else{
		v_file_query.cmd = V_FILE_TABLE_QUERY_INFO_BY_UUID;
		S_STRNCPY(v_file_query.v_file_info.uuid,sObject->file_uuid,MAX_FILE_UUID_LEN);
	}
	int res = _handle_v_file_table_query(&v_file_query);
	if(res != 0)
	{
		DMCLOG_E("query v file table by uuid error");
		return -1;
	}
	return 0;
}


/**
 * ��Ͱ����
 */
int _bfavfs_fcopy(BucketObject* sObject,BucketObject* dObject,void * token)
{
	if(sObject == NULL||dObject == NULL||token == NULL)
	{
		DMCLOG_E("para is null");
		return -1;
	}
	
	v_file_update_t v_file_update;
	memset(&v_file_update,0,sizeof(v_file_update_t));
	S_STRNCPY(v_file_update.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
	S_STRNCPY(v_file_update.v_file_info.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
	S_STRNCPY(v_file_update.v_des_info.path,dObject->path,MAX_FILE_PATH_LEN);
	S_STRNCPY(v_file_update.v_des_info.bucket_name,dObject->bucket_name,MAX_BUCKET_NAME_LEN);
	
	v_file_update.cmd = V_FILE_TABLE_UPDATE_FILE_COPY;
	v_file_update.remove = rfsvfs_remove;
	int res = _handle_v_file_table_update(&v_file_update);
	if(res != 0)
	{
		DMCLOG_D("copy v file table error,src_path = %s,des_path:%s",v_file_update.v_des_info.path,v_file_update.v_file_info.path);
		return -1;
	}
	DMCLOG_D("src_path = %s,des_path:%s",v_file_update.v_des_info.path,v_file_update.v_file_info.path);
	return 0;
}

/**
 * ��Ͱ�ƶ�
 */
int _bfavfs_fmove(BucketObject* sObject,BucketObject* dObject,void * token)
{
	if(sObject == NULL||dObject == NULL||token == NULL)
	{
		DMCLOG_E("para is null");
		return -1;
	}
	
	v_file_update_t v_file_update;
	memset(&v_file_update,0,sizeof(v_file_update_t));
	S_STRNCPY(v_file_update.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
	S_STRNCPY(v_file_update.v_file_info.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
	S_STRNCPY(v_file_update.v_des_info.path,dObject->path,MAX_FILE_PATH_LEN);
	S_STRNCPY(v_file_update.v_des_info.bucket_name,dObject->bucket_name,MAX_BUCKET_NAME_LEN);

	v_file_update.cmd = V_FILE_TABLE_UPDATE_FILE_MOVE;
	v_file_update.remove = rfsvfs_remove;
	int res = _handle_v_file_table_update(&v_file_update);
	if(res != 0)
	{
		DMCLOG_D("move v file table error,src_path = %s,des_path:%s",v_file_update.v_des_info.path,v_file_update.v_file_info.path);
		return -1;
	}
	DMCLOG_D("src_path = %s,des_path:%s",v_file_update.v_des_info.path,v_file_update.v_file_info.path);
	return 0;
}

/**
 * �ļ�������
 */
int _bfavfs_frename(BucketObject* sObject,BucketObject* dObject,void * token)
{
	if(sObject == NULL||dObject == NULL||token == NULL)
	{
		DMCLOG_E("para is null");
		return -1;
	}
	
	v_file_update_t v_file_update;
	memset(&v_file_update,0,sizeof(v_file_update_t));
	S_STRNCPY(v_file_update.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
	S_STRNCPY(v_file_update.v_file_info.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
	S_STRNCPY(v_file_update.v_des_info.path,dObject->path,MAX_FILE_PATH_LEN);
	S_STRNCPY(v_file_update.v_des_info.bucket_name,dObject->bucket_name,MAX_BUCKET_NAME_LEN);
	S_STRNCPY(v_file_update.bucket_name,dObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_update.cmd = V_FILE_TABLE_UPDATE_FILE_RENAME;
	v_file_update.remove = rfsvfs_remove;
	int res = _handle_v_file_table_update(&v_file_update);
	if(res != 0)
	{
		DMCLOG_D("rename v file table error,src_path = %s,des_path:%s",v_file_update.v_des_info.path,v_file_update.v_file_info.path);
		return -1;
	}
	DMCLOG_D("src_path = %s,des_path:%s",v_file_update.v_des_info.path,v_file_update.v_file_info.path);
	return 0;
}

/**
 * ɾ��Ŀ¼�������ļ�
 
 */
int _bfavfs_remove(BucketObject* sObject,void * token)
{
	if(sObject == NULL || token == NULL)
	{
		DMCLOG_E("para is null");
		return -1;
	}
	v_file_delete_t v_file_delete;
	memset(&v_file_delete,0,sizeof(v_file_delete_t));
	S_STRNCPY(v_file_delete.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
	S_STRNCPY(v_file_delete.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
	if(sObject->file_type > 0)
	{
		v_file_delete.cmd = V_FILE_TABLE_DELETE_TYPE_BY_PATH;
		v_file_delete.v_file_info.type = sObject->file_type;
	}else{
		v_file_delete.cmd = V_FILE_TABLE_DELETE_INFO;
	}
	
	v_file_delete.remove = rfsvfs_remove;
	
	int res = _handle_v_file_table_delete(&v_file_delete);
	if(res != 0)
	{
		DMCLOG_E("delete v file table error");
		return -1;
	}
	DMCLOG_D("path:%s",v_file_delete.v_file_info.path);
	return 0;
}

/**
 * ɾ��Ŀ¼�������ļ�
 
 */
int bfavfs_remove_type(char* path,int file_type,void * token)
{
	if(path == NULL || token == NULL||file_type < 0)
	{
		DMCLOG_E("para is null");
		return -1;
	}
	token_dnode_t *token_dnode = (token_dnode_t *)token;
	v_file_info_t v_file_info;
	memset(&v_file_info,0,sizeof(v_file_info_t));
	if(*path)
	{
		sprintf(v_file_info.path,"/%s/%s",token_dnode->bucket_name,path);
	}else{
		sprintf(v_file_info.path,"/%s",token_dnode->bucket_name);
	}

	v_file_info.type = file_type;
	
	int res = handle_v_file_table_delete(token_dnode->bucket_name,V_FILE_TABLE_DELETE_TYPE_BY_PATH,rfsvfs_remove,&v_file_info);
	if(res != 0)
	{
		DMCLOG_E("delete v file table error");
		return -1;
	}
	DMCLOG_D("path:%s",v_file_info.path);
	return 0;
}

/**
 * �����ļ���
 
 */
int _bfavfs_mkdir(BucketObject* sObject,void * token)
{
	int res = 0;
	v_file_insert_t v_file_insert;
	memset(&v_file_insert,0,sizeof(v_file_insert_t));
	v_file_insert.cmd = V_FILE_TABLE_INSERT_INFO;
	S_STRNCPY(v_file_insert.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
	S_STRNCPY(v_file_insert.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
	time_t now; //ʵ����time_t�ṹ    
	struct tm *timenow; //ʵ����tm�ṹָ��    
	time(&now);
	//time������ȡ���ڵ�ʱ��(���ʱ�׼ʱ��Ǳ���ʱ��)��Ȼ��ֵ��now    
	v_file_insert.v_file_info.atime = now;
	v_file_insert.v_file_info.ctime = now;
	v_file_insert.v_file_info.mtime = now;
	
	v_file_insert.v_file_info.isDir = 1;
	
	res = _handle_v_file_table_insert(&v_file_insert);
	if(res != 0)
	{
		DMCLOG_E("mkdir file table error");
		return NULL;
	}
	return 0;
}



/**
 * �����ļ�����
 */
int bfavfs_fsetattr(const char *path,void *token)
{
	ENTER_FUNC();
	token_dnode_t *token_dnode = (token_dnode_t *)token;
	v_file_info_t v_file_info;
	memset(&v_file_info,0,sizeof(v_file_info_t));
	char real_path[32] = {0};
	char db_path[MAX_FILE_PATH_LEN] = {0};
	if(*path)
	{
		sprintf(db_path,"/%s/%s",token_dnode->bucket_name,path);
	}else{
		sprintf(db_path,"/%s",token_dnode->bucket_name);
	}
	S_STRNCPY(v_file_info.path,db_path,MAX_FILE_PATH_LEN);
	int res = handle_v_file_table_query(token_dnode->bucket_name,&v_file_info);
	if(res != 0)
	{
		DMCLOG_E("query v file table error");
		return -1;
	}
	
	if(*v_file_info.real_path)
	{
		DMCLOG_D("real_path:%s",v_file_info.real_path);
		//TODO for xwm
		#if 0
		add_media_to_list(v_file_info.real_path,v_file_info.type,media_prc_thpool_cb,0);
		#endif
	}else{
		DMCLOG_E("the %s is not exist",path);
		return -1;
	}
	
	EXIT_FUNC();
	return 0;
}

/**
 * �����ļ�����
 */
int _bfavfs_fsetattr(BucketObject* sObject,void *token)
{
	ENTER_FUNC();
	v_file_query_t v_file_query;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,sObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,sObject->path,MAX_FILE_PATH_LEN);
	int res = _handle_v_file_table_query(&v_file_query);
	if(res != 0)
	{
		DMCLOG_E("query v file table error");
		return -1;
	}
	
	if(*v_file_query.v_file_info.real_path)
	{
		DMCLOG_D("real_path:%s",v_file_query.v_file_info.real_path);
		//TODO for xwm
		#if 0
		char tmp_path[1024];
		memset(tmp_path,0,sizeof(tmp_path));
		strcpy(tmp_path,"/tmp/mnt/SD-disk-1");
		strcat(tmp_path,v_file_info.real_path);
		DMCLOG_D("v_file_info.type:%d",v_file_info.type);
		add_media_to_list(tmp_path,v_file_info.type,media_prc_thpool_cb,0);
		#endif
	}else{
		DMCLOG_E("the %s is not exist",v_file_query.v_file_info.real_path);
		return -1;
	}
	EXIT_FUNC();
	return 0;
}


/**
 *����fd�Ǹý��̴������ļ��������� �����ɹ�ִ��ʱ������0��ʧ�ܷ���-1��errno����Ϊ���µ�ĳ��ֵ
EBADF�� �ļ���������Ч
EIO �� ��д�Ĺ����з�������
EROFS, EINVAL���ļ����ڵ��ļ�ϵͳ��֧��ͬ��
 */
int _bfavfs_fsync( VFILE *vf,void* token)
{
	ENTER_FUNC();
	if(vf == NULL || vf->fp == NULL)
	{
		DMCLOG_E("para is null");
		return -1;
	}
	int fd = fileno(vf->fp);
	if(fd < 0)
	{
		DMCLOG_E("invalid file descriptor");
		return -1;
	}
	return fsync(fd);
}

/**
 *ftruncate()�Ὣ����fdָ�����ļ���С��Ϊ����lengthָ���Ĵ�С��
��������fdΪ�Ѵ򿪵��ļ������ʣ����ұ�������д��ģʽ�򿪵��ļ���
�������ԭ�����ļ���С�Ȳ���length���򳬹��Ĳ��ֻᱻɾȥ��
 *��������ֵ:ִ�гɹ��򷵻�0��ʧ�ܷ���-1������ԭ�����errno��
 */
int _bfavfs_ftruncate( VFILE *vf,off_t length,void* token)
{
	if(vf == NULL || vf->fp == NULL)
	{
		DMCLOG_E("para is null");
		return -1;
	}
	int fd = fileno(vf->fp);
	if(fd < 0)
	{
		DMCLOG_E("invalid file descriptor");
		return -1;
	}
	return ftruncate(fd,length);
}


/**
 *fallocate�Ĺ�����Ϊ�ļ�Ԥ��������ռ䡣
 *����ֵ:ִ�гɹ��򷵻�0��ʧ�ܷ���-1������ԭ�����errno��
 */
int _bfavfs_fallocate( VFILE *vf, int mode, off_t offset, off_t len,void* token)
{
	if(vf == NULL || vf->fp == NULL)
	{
		DMCLOG_E("para is null");
		return -1;
	}
	int fd = fileno(vf->fp);
	if(fd < 0)
	{
		DMCLOG_E("invalid file descriptor");
		return -1;
	}
	return lseek(fd, len,offset);
}

/**
 *link()�Բ���newpath ָ��������������һ���µ�����(Ӳ����)������oldpath ��ָ�����Ѵ����ļ�. �������newpath ָ��������Ϊһ�Ѵ��ڵ��ļ��򲻻Ὠ������.
 *����ֵ���ɹ��򷵻�0, ʧ�ܷ���-1, ����ԭ�����errno.
 */
int _bfavfs_link( BucketObject* oObject, BucketObject* nObject,void* token)
{
	char new_path[32] = {0};
	char old_path[32] = {0};
	v_file_query_t v_file_query;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,oObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,oObject->path,MAX_FILE_PATH_LEN);
	int res = _handle_v_file_table_query(&v_file_query);
	if(res != 0)
	{
		DMCLOG_E("the old file %s is not exist",v_file_query.v_file_info.path);
		return -1;
	}
	S_STRNCPY(old_path,v_file_query.v_file_info.real_path,32);
	
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,nObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,nObject->path,MAX_FILE_PATH_LEN);
	res = _handle_v_file_table_query(&v_file_query);
	if(res == 0)
	{
		DMCLOG_E("the new file %s is exist",v_file_query.v_file_info.path);
		return -1;
	}else{
		res =  rfsvfs_get_new_file_path(new_path);
		if(res != 0)
		{
			DMCLOG_E("get new file path error");
			return -1;
		}
		// 2 :insert new path to file table
		v_file_insert_t v_file_insert;
		memset(&v_file_insert,0,sizeof(v_file_insert_t));
		v_file_insert.cmd = V_FILE_TABLE_INSERT_INFO;
		S_STRNCPY(v_file_insert.bucket_name,nObject->bucket_name,MAX_BUCKET_NAME_LEN);
		DMCLOG_D("real path:%s,bucket_name = %s",new_path,nObject->bucket_name);
		S_STRNCPY(v_file_insert.v_file_info.path,nObject->path,MAX_FILE_PATH_LEN);
		S_STRNCPY(v_file_insert.v_file_info.real_path,new_path,MAX_REAL_PATH_LEN);
		DMCLOG_D("path = %s,real_path = %s",v_file_insert.v_file_info.path,new_path);
		res = _handle_v_file_table_insert(&v_file_insert);
		if(res != 0)
		{
			DMCLOG_E("insert file table error");
			return -1;
		}
	}
	return link (old_path, new_path);
}

/**
 *readlink()�Ὣ����path �ķ����������ݴ浽����buf ��ָ���ڴ�ռ�, ���ص����ݲ�����NULL���ַ�����β, ���Ὣ�ַ������ַ�������. ������bufsiz С�ڷ������ӵ����ݳ���, ���������ݻᱻ�ض�.
 *����ֵ��ִ�гɹ��򴫷���������ָ���ļ�·���ַ���, ʧ���򷵻�-1, ����������errno.
 */
int _bfavfs_readlink( BucketObject* rObject,char * buf, size_t bufsiz,void* token)
{
	char real_path[32] = {0};
	v_file_query_t v_file_query;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,rObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,rObject->path,MAX_FILE_PATH_LEN);
	int res = _handle_v_file_table_query(&v_file_query);
	if(res != 0)
	{
		DMCLOG_E("the old file %s is not exist",v_file_query.v_file_info.path);
		return -1;
	}
	DMCLOG_D("real_path = %s",v_file_query.v_file_info.real_path);
	S_STRNCPY(real_path,v_file_query.v_file_info.real_path,32);
	
	return readlink (real_path,buf,bufsiz);
}

/**
 *symlink()�Բ���newpath ָ��������������һ���µ�����(��������)������oldpath ��ָ�����Ѵ����ļ�. ����oldpath ָ�����ļ���һ��Ҫ����, �������newpath ָ��������Ϊһ�Ѵ��ڵ��ļ��򲻻Ὠ������.
 *����ֵ���ɹ��򷵻�0, ʧ�ܷ���-1, ����ԭ�����errno.
 */
int _bfavfs_symlink( BucketObject* oObject, BucketObject* nObject,void* token)
{
	char new_path[32] = {0};
	char old_path[32] = {0};
	v_file_query_t v_file_query;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,oObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,oObject->path,MAX_FILE_PATH_LEN);
	int res = _handle_v_file_table_query(&v_file_query);
	if(res != 0)
	{
		DMCLOG_E("the old file %s is not exist",v_file_query.v_file_info.path);
		res =  rfsvfs_get_new_file_path(old_path);
		if(res != 0)
		{
			DMCLOG_E("get new file path error");
			return -1;
		}
		// 2 :insert new path to file table
		v_file_insert_t v_file_insert;
		memset(&v_file_insert,0,sizeof(v_file_insert_t));
		v_file_insert.cmd = V_FILE_TABLE_INSERT_INFO;
		S_STRNCPY(v_file_insert.bucket_name,oObject->bucket_name,MAX_BUCKET_NAME_LEN);
		DMCLOG_D("old path:%s,bucket_name = %s",old_path,oObject->bucket_name);
		S_STRNCPY(v_file_insert.v_file_info.path,nObject->path,MAX_FILE_PATH_LEN);
		S_STRNCPY(v_file_insert.v_file_info.real_path,old_path,MAX_REAL_PATH_LEN);
		DMCLOG_D("path = %s,old_path = %s",v_file_insert.v_file_info.path,old_path);
		res = _handle_v_file_table_insert(&v_file_insert);
		if(res != 0)
		{
			DMCLOG_E("insert file table error");
			return -1;
		}
	}else{
		S_STRNCPY(old_path,v_file_query.v_file_info.real_path,32);
	}
	DMCLOG_D("old_path = %s",old_path);
	
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,nObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,nObject->path,MAX_FILE_PATH_LEN);
	res = _handle_v_file_table_query(&v_file_query);
	if(res == 0)
	{
		DMCLOG_E("the new file %s is exist",v_file_query.v_file_info.path);
		return -1;
	}else{
		res =  rfsvfs_get_new_file_path(new_path);
		if(res != 0)
		{
			DMCLOG_E("get new file path error");
			return -1;
		}
		// 2 :insert new path to file table
		v_file_insert_t v_file_insert;
		memset(&v_file_insert,0,sizeof(v_file_insert_t));
		v_file_insert.cmd = V_FILE_TABLE_INSERT_INFO;
		S_STRNCPY(v_file_insert.bucket_name,nObject->bucket_name,MAX_BUCKET_NAME_LEN);
		DMCLOG_D("real path:%s,bucket_name = %s",new_path,nObject->bucket_name);
		S_STRNCPY(v_file_insert.v_file_info.path,nObject->path,MAX_FILE_PATH_LEN);
		S_STRNCPY(v_file_insert.v_file_info.real_path,new_path,MAX_REAL_PATH_LEN);
		DMCLOG_D("path = %s,real_path = %s",v_file_insert.v_file_info.path,new_path);
		res = _handle_v_file_table_insert(&v_file_insert);
		if(res != 0)
		{
			DMCLOG_E("insert file table error");
			return -1;
		}
	}
	return symlink (old_path, new_path);
}
//����futimensat�����ͨ���ƶ�һ���ļ��ľ���·�����޸��ļ���ʱ�����ԣ�����ʱfd���������ԡ�
int _bfavfs_utimensat( BucketObject* rObject,const struct timespec times[2], int flag,void* token)
{
	char real_path[32] = {0};
	v_file_query_t v_file_query;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,rObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,rObject->path,MAX_FILE_PATH_LEN);
	int res = _handle_v_file_table_query(&v_file_query);
	if(res != 0)
	{
		DMCLOG_E("the old file %s is not exist",v_file_query.v_file_info.path);
		return -1;
	}
	DMCLOG_D("real_path = %s",v_file_query.v_file_info.real_path);
	S_STRNCPY(real_path,v_file_query.v_file_info.real_path,32);
	
	if(rfsvfs_utimensat(0 ,real_path,times,flag) != 0)
	{
		DMCLOG_E("utime error, errno[%d %s]", errno, strerror(errno));
		return -1;
	}

	v_file_update_t v_file_update;
	memset(&v_file_update,0,sizeof(v_file_update_t));
	S_STRNCPY(v_file_update.v_file_info.path,rObject->path,MAX_FILE_PATH_LEN);
	DMCLOG_D("path = %s",rObject->path);
	S_STRNCPY(v_file_update.bucket_name,rObject->bucket_name,MAX_BUCKET_NAME_LEN);
	DMCLOG_D("bucket_name = %s",v_file_update.bucket_name);

	struct stat st;
	DMCLOG_D("real_path = %s",real_path);
	if(rfsvfs_fstat(real_path,&st) != 0)
	{
		DMCLOG_E("stat error");
		
		return -1;
	}
	v_file_update.v_file_info.atime = st.st_atime;
	v_file_update.v_file_info.ctime = st.st_ctime;
	v_file_update.v_file_info.mtime = st.st_mtime;
	v_file_update.v_file_info.size = st.st_size;
	DMCLOG_D("st.st_size = %lld,v_file_update.v_file_info.size = %lld",(long long)st.st_size,(long long)v_file_update.v_file_info.size);
	v_file_update.v_file_info.isDir = S_ISDIR(st.st_mode);
	v_file_update.v_file_info.type =  db_get_mime_type(rObject->path,strlen(rObject->path));
	if(*v_file_query.v_file_info.uuid)
	{
		S_STRNCPY(v_file_update.v_file_info.uuid,v_file_query.v_file_info.uuid,MAX_FILE_UUID_LEN);
	}else{
		strcpy(v_file_update.v_file_info.uuid,"13141314");
	}
	v_file_update.cmd = V_FILE_TABLE_UPDATE_FILE_INFO;
	v_file_update.remove = rfsvfs_remove;
	res = _handle_v_file_table_update(&v_file_update);
	if(res != 0)
	{
		DMCLOG_E("update file table error");
		return -1;
	}
	return 0;
}

//��ȡ�ļ�ϵͳ��Ϣ
int _bfavfs_statvfs( BucketObject* rObject, struct statvfs *buf, void* token)
{
	char real_path[32] = {0};
	v_file_query_t v_file_query;
	memset(&v_file_query,0,sizeof(v_file_query_t));
	S_STRNCPY(v_file_query.bucket_name,rObject->bucket_name,MAX_BUCKET_NAME_LEN);
	v_file_query.cmd = V_FILE_TABLE_QUERY_INFO;
	S_STRNCPY(v_file_query.v_file_info.path,rObject->path,MAX_FILE_PATH_LEN);
	int res = _handle_v_file_table_query(&v_file_query);
	if(res != 0)
	{
		DMCLOG_E("the old file %s is not exist",v_file_query.v_file_info.path);
		return -1;
	}
	DMCLOG_D("real_path = %s",v_file_query.v_file_info.real_path);
	S_STRNCPY(real_path,v_file_query.v_file_info.real_path,32);
	
	return rfsvfs_statvfs(real_path, buf);
}



