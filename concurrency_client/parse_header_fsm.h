#ifndef PARSE_HEADER_FSM_H
#define PARSE_HEADER_FSM_H

enum LINE_STATE{LINE_OK, LINE_ING, LINE_BAD  };


extern enum LINE_STATE
parse_line(char *p_buf, int *p_checked, int *p_readed, int *p_begline);







#endif

