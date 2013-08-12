#include <stdlib.h>
#include <stdio.h>

/* interpreter state and setup */
void sl_init();
void sl_init_type();

/* types */
typedef void * sl_value;

struct SLBasic
{
        sl_value type;
};

struct SLType
{
        struct SLBasic basic;
        char *name;
};

struct SLInteger
{
        struct SLBasic basic;
        long value;
};

extern sl_value sl_Type;
extern sl_value sl_Integer;

#define SL_BASIC(v)   ((struct SLBasic*)(v))
#define SL_TYPE(v)    ((struct SLType*)(v))
#define SL_INTEGER(v) ((struct SLInteger*)(v))

sl_value sl_new_type(char *name);

/* gc - haha */
#define sl_alloc(type) (type*)malloc(sizeof(type))

/* reader */
sl_value sl_read_string(char *input);

/* numbers */
sl_value sl_new_integer(int i);

#define NUM2INT(n) SL_INTEGER(n)->value
