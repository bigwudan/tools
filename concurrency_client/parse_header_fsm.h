#ifndef PARSE_HEADER_FSM_H
#define PARSE_HEADER_FSM_H

enum LINE_STATE{LINE_OK, LINE_ING, LINE_BAD  };
enum HEAD_VAL_TYPE { STRING, INT };
enum HEAD_TYPE{HTTP_HEAD_TYPE, PROTOCOL_HEAD_TYPE, BAD_HEAD_TYPE, HTTP_BODY_TYPE };


extern enum LINE_STATE
parse_line(char *p_buf, int *p_checked, int *p_readed, int *p_begline);


union head_value_variant{
    int i_val;
    char *p_c_val;
};

struct head_param{
    char *p_name;
    enum HEAD_VAL_TYPE val_type;
    union head_value_variant val;
    int len;
};


extern struct head_param head_param_list[]; 
extern enum HEAD_TYPE parse_http_head(char *p_buf);
extern enum HEAD_TYPE parse_proto_head(char *p_buf);

extern int parse_run(char *p_m_f);

#endif

