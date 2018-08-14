#ifndef _SHTTPD_H_
#define _SHTTPD_H_
#define JUMPTO_CHAR(pos, s_char) do{ for(; *pos != s_char; pos++);  }while(0);
#define JUMPOVER_CHAR(pos, s_char) do{ for(; *pos == s_char; pos++);   }while(0);
#define OFFSET(x) offsetof(struct headers, x)


typedef enum SHTTPD_METHOD_TYPE{
	METGOD_GET,
	METGOD_POST,
	METGOD_PUT,
	METGOD_DELETE,
	METGOD_HEAD
}SHTTPD_METHOD_TYPE;

typedef struct vec{
	char *ptr;
	int len;
	SHTTPD_METHOD_TYPE type;
}vec;



union variant{
	char *v_str;
	int v_int;
	struct vec v_vec;

};

struct headers{
	union variant cl;
	union variant ct;
	union variant connection;
	union variant ims;
	union variant user;
	union variant auth;
	union variant useragent;
	union variant referer;
	union variant cookie;
};

struct http_header{
	int len;
	int type;
	long unsigned int offset;
	char *name;
};

enum {HDR_DATE, HDR_INT, HDR_STRING};

struct http_header http_headers[] = {

	{16, HDR_INT, OFFSET(cl), "Content-Length: "},
	{14, HDR_STRING, OFFSET(ct), "Content-Type: "},
	{12, HDR_STRING, OFFSET(useragent), "User-Agent: "},
	{0, HDR_INT, 0 , NULL}




};



struct conn_request{
	struct vec req;
	char *head;
	char *uri;
	char rpath[1024];
	char rqs[4012];
	int len;
	int method;
	unsigned long major;
	unsigned long minor;
	struct headers ch;
};

struct vec _shttpd_methods[] = 
{
	{"GET", 3, METGOD_GET   },
	{"POST", 4, METGOD_POST},
	{NULL, 0, 0}
};







#endif 
