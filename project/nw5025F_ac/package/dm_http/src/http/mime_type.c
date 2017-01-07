/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

#include "defs.h"

/*static const struct mime_type default_mime_types[] = {
	{"html",	4,	"text/html"			},
	{"htm",		3,	"text/html"			},
	{"txt",		3,	"text/plain"			},
	{"css",		3,	"text/css"			},
	{"ico",		3,	"image/x-icon"			},
	{"gif",		3,	"image/gif"			},
	{"jpg",		3,	"image/jpeg"			},
	{"jpeg",	4,	"image/jpeg"			},
	{"png",		3,	"image/png"			},
	{"svg",		3,	"image/svg+xml"			},
	{"torrent",	7,	"application/x-bittorrent"	},
	{"wav",		3,	"audio/x-wav"			},
	{"mp3",		3,	"audio/x-mp3"			},
	{"mid",		3,	"audio/mid"			},
	{"m3u",		3,	"audio/x-mpegurl"		},
	{"ram",		3,	"audio/x-pn-realaudio"		},
	{"ra",		2,	"audio/x-pn-realaudio"		},
	{"doc",		3,	"application/msword",		},
	{"exe",		3,	"application/octet-stream"	},
	{"zip",		3,	"application/x-zip-compressed"	},
	{"xls",		3,	"application/excel"		},
	{"tgz",		3,	"application/x-tar-gz"		},
	{"tar.gz",	6,	"application/x-tar-gz"		},
	{"tar",		3,	"application/x-tar"		},
	{"gz",		2,	"application/x-gunzip"		},
	{"arj",		3,	"application/x-arj-compressed"	},
	{"rar",		3,	"application/x-arj-compressed"	},
	{"rtf",		3,	"application/rtf"		},
	{"pdf",		3,	"application/pdf"		},
	{"mpg",		3,	"video/mpeg"			},
	{"mpeg",	4,	"video/mpeg"			},
	{"asf",		3,	"video/x-ms-asf"		},
	{"avi",		3,	"video/x-msvideo"		},
	{"bmp",		3,	"image/bmp"			},
	{NULL,		0,	NULL				}
};*/

static const struct mime_type default_mime_types[] = {
    {"html",	4,	"text/html"			,4},
    {"htm",		3,	"text/html"			,4},
    {"txt",		3,	"text/plain"		,4	},
    {"css",		3,	"text/css"			,4},
    //{"rar",		3,	"text/rar"			,4},
    //{"zip",		3,	"text/zip"			,4},
	//{"apk",		3,	"text/apk"			,4},
    //{"docx",	4,	"text/docx"			,4},
	{"docx",	4,	"application/vnd.openxmlformats-officedocument.wordprocessingml.document"			,4},
	{"wps",		4,	"application/vnd.ms-works"	,4},
	//{"ppt",		3,	"text/ppt"			,4},
    //{"pptx",	4,	"text/pptx"			,4},
	{"ppt",		3,	"application/vnd.ms-powerpoint"			,4},
    {"pptx",	4,	"application/vnd.ms-powerpoint"			,4},
    {"pps",		3,	"application/vnd.ms-powerpoint"			,4},
    {"pot",		3,	"application/vnd.ms-powerpoint"			,4},
	{"bmp",		3,	"image/bmp"			,3},
    {"ico",		3,	"image/x-icon"		,3},
    {"gif",		3,	"image/gif"			,3},
    {"jpg",		3,	"image/jpeg"		,3},
    {"jpeg",	4,	"image/jpeg"		,3},
    {"png",		3,	"image/png"			,3},
    {"pcx",		3,	"image/pcx"			,3},
    {"tga",		3,	"image/x-targa"		,3},
    {"svg",		3,	"image/svg+xml"		,3},
    {"tif",		3,	"image/tif"		,3	},
    {"torrent",	7,	"application/x-bittorrent"	,4},
    {"wav",		3,	"audio/x-wav"			,2},
    {"mp3",		3,	"audio/x-mp3"			,2},
    {"mid",		3,	"audio/x-mid"			,2},
    {"m3u",		3,	"audio/x-mpegurl"	,2},
    {"ram",		3,	"audio/x-pn-realaudio"		,2},
    {"ra",		2,	"audio/x-pn-realaudio"		,2},
    {"ac3",		3,	"audio/ac3"			,2},
    {"wma",		3,	"audio/wma"			,2},
    {"flac",	4,	"audio/flac"			,2},
    {"aac",		3,	"audio/aac"			,2},
	{"m4a", 	3,	"audio/x-m4a"			,2},
	{"m4r", 	3,	"audio/x-m4r"			,2},
	{"ram",		3,	"audio/vnd.rn-realaudio"	,2},
    {"ra",		2,	"audio/vnd.rn-realaudio"	,2},
    {"mp2",		3,	"audio/x-mpeg"				,2},
    {"ogg",		3,	"audio/x-ogg"				,2},
    {"m3u",		3,	"audio/mpegurl"				,2},
    {"ac3",		3,	"audio/ac3"					,2},
    {"ape",		3,	"audio/ape"					,2},
    {"dts",		3,	"audio/vnd.dts"				,2},
    {"mmf",		3,	"application/vnd.smaf"		,0},
    {"doc",		3,	"application/msword"		,4},
    {"exe",		3,	"application/octet-stream"	,0},
    {"zip",		3,	"application/x-zip-compressed"	,0},
    {"rar",		3,	"application/x-rar-compressed"	,0},
    {"zip",		3,	"application/x-zip-compressed"	,0},
	{"apk",		3,	"application/vnd.android.package-archive"	,0},
	{"ipa",		3,	"pplication/octet-stream"	,0},
    //{"xls",		3,	"application/excel"		,0},    
	{"xls", 	3,	"application/vnd.ms-excel" 	,0},
	{"xlm", 	3,	"application/vnd.ms-excel" 	,0},
	{"xla", 	3,	"application/vnd.ms-excel" 	,0},
	{"xlc", 	3,	"application/vnd.ms-excel" 	,0},
	{"xlt", 	3,	"application/vnd.ms-excel" 	,0},
	{"xlw", 	3,	"application/vnd.ms-excel" 	,0},
	{"xlsx", 	4,	"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" 	,0},
    {"tgz",		3,	"application/x-tar-gz"		,0},
    {"tar.gz",	6,	"application/x-tar-gz"		,0},
    {"tar",		3,	"application/x-tar"		,0},
    {"gz",		2,	"application/x-gunzip"		,0},
    {"arj",		3,	"application/x-arj-compressed"	,0},
    {"rar",		3,	"application/x-arj-compressed"	,0},
    {"rtf",		3,	"application/rtf"		,0},
    {"plist",	5,	"application/x-plist"	,0},
    {"strings",	7,	"application/octet-stream",	0},
    {"pdf",		3,	"application/pdf"		,4},
    {"mpg",		3,	"video/mpeg"			,1},
    {"mpeg",	4,	"video/mpeg"			,1},
    {"asf",		3,	"video/x-ms-asf"		,1},
    //{"avi",		3,	"video/x-msvideo"		,1},
    {"avi",		3,	"video/avi"		,1},
    //{"mp4",		3,	"video/x-msvideo"		,1},
	{"mp4",		3,	"video/mp4"				,1},
	{"3gp",		3,	"video/x-msvideo"		,1},
    //{"mkv",		3,	"video/x-msvideo"		,1},
    {"mkv",		3,	"video/realvideo"		,1},
	//{"flv",		3,	"video/x-msvideo"		,1},
	{"flv",		3,	"video/x-flv"			,1},
	//{"mov",		3,	"video/x-msvideo"		,1},
    //{"mov",		3,	"video/x-sgi-movie"		,1},
    {"mov",		3,	"video/quicktime"		,1},
	//{"rmvb",	4,	"video/x-msvideo"		,1},
    {"rmvb",	4,	"application/vnd.rn-realmedia-vbr"		,1},
    {"ts",		3,	"video/x-msvideo"		,1},
    //{"wmv",		3,	"video/x-msvideo"		,1},
    {"wmv",		3,	"video/x-ms-wmv"		,1},
    {"vob",		3,	"video/x-msvideo"		,1},
    {"swf",		3,	"application/x-shockwave-flash"		,1},
    {"m2ts",	4,	"video/x-msvideo"		,1},
    {"m4v",		3,	"video/x-m4v"			,1},
    {"mpv",		3,	"video/mpg"				,1},
    {"mpv2",	4,	"video/mpeg"			,1},
    {"dat",		3,	"video/x-msvideo"		,1},
    {"tp",		2,	"video/tp"				,1},
    {"bin",		3,	"application/octet-stream"	,0},
    {NULL,		0,	NULL				}
};


const char *
get_mime_type(struct shttpd_ctx *ctx, const char *uri, int len)
{
	struct llhead		*lp;
	const struct mime_type	*mt;
	struct mime_type_link	*mtl;
	const char		*s;

	/* Firt, loop through the custom mime types if any */
	LL_FOREACH(&ctx->mime_types, lp) {
		mtl = LL_ENTRY(lp, struct mime_type_link, link);
		s = uri + len - mtl->ext_len;
		if (s > uri && s[-1] == '.' &&
		    !my_strncasecmp(mtl->ext, s, mtl->ext_len))
			return (mtl->mime);
	}

	/* If no luck, try built-in mime types */
	for (mt = default_mime_types; mt->ext != NULL; mt++) {
		s = uri + len - mt->ext_len;
		if (s > uri && s[-1] == '.' &&
		    !my_strncasecmp(mt->ext, s, mt->ext_len))
			return (mt->mime);
	}

	/* Oops. This extension is unknown to us. Fallback to text/plain */
	return ("text/plain");
}

void
set_mime_types(struct shttpd_ctx *ctx, const char *path)
{
	FILE	*fp;
	char	line[512], ext[sizeof(line)], mime[sizeof(line)], *s;

	if ((fp = fopen(path, "r")) == NULL)
		elog(E_FATAL, NULL, "set_mime_types: fopen(%s): %s",
		    path, strerror(errno));

	while (fgets(line, sizeof(line), fp) != NULL) {
		/* Skip empty lines */
		if (line[0] == '#' || line[0] == '\n')
			continue;
		if (sscanf(line, "%s", mime)) {
			s = line + strlen(mime);
			while (*s && *s != '\n' && sscanf(s, "%s", ext)) {
				shttpd_add_mime_type(ctx, ext, mime);
				s += strlen(mime);
			}
		}
	}

	(void) fclose(fp);
}

int dm_get_mime_type(const char *uri, int len)
{
	struct llhead		*lp;
	const struct mime_type	*mt;
	const char		*s;
	/* If no luck, try built-in mime types */
	for (mt = default_mime_types; mt->ext != NULL; mt++) {
		s = uri + len - mt->ext_len;
		if (s > uri && s[-1] == '.' &&
		    !my_strncasecmp(mt->ext, s, mt->ext_len))
			return mt->file_type;
	}
	/* Oops. This extension is unknown to us. Fallback to text/plain */
	return 0;
}

