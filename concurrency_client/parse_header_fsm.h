#ifndef PARSE_HEADER_FSM_H
#define PARSE_HEADER_FSM_H

enum LINE_STATE{LINE_OK, LINE_ING, LINE_BAD  };
enum HEAD_VAL_TYPE { STRING, INT };

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



#endif

