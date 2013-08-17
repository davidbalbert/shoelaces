#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "khash.h"

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

struct SLSymbol
{
        struct SLBasic basic;
        char *name;
};

struct SLInteger
{
        struct SLBasic basic;
        long value;
};

struct SLBoolean
{
        struct SLBasic basic;
        char value;
};

struct SLList
{
        struct SLBasic basic;
        sl_value first;
        sl_value rest;
        sl_value size;
};

#define SL_BASIC(v)   ((struct SLBasic*)(v))
#define SL_TYPE(v)    ((struct SLType*)(v))
#define SL_SYMBOL(v)  ((struct SLSymbol*)(v))
#define SL_INTEGER(v) ((struct SLInteger*)(v))
#define SL_BOOLEAN(v) ((struct SLBoolean*)(v))
#define SL_LIST(v)    ((struct SLList*)(v))

extern sl_value sl_tType;
extern sl_value sl_tSymbol;
extern sl_value sl_tInteger;
extern sl_value sl_tBoolean;
extern sl_value sl_tList;

sl_value sl_new_type(char *name);
sl_value sl_type(sl_value object);


/* interpreter state and setup */
KHASH_MAP_INIT_STR(str, sl_value);

struct sl_interpreter_state {
        khash_t(str) *symbol_table;
};

struct sl_interpreter_state *sl_init();
void sl_destroy(struct sl_interpreter_state *state);

void sl_init_type();
void sl_init_symbol();
void sl_init_number();
void sl_init_boolean();
void sl_init_list();

sl_value sl_symbol_table_get(struct sl_interpreter_state *state, char *name);
void sl_symbol_table_put(struct sl_interpreter_state *state, char *name, sl_value value);


/* gc - haha */
#define sl_alloc(type) (type*)malloc(sizeof(type))

/* reader */
sl_value sl_read(struct sl_interpreter_state *state, char *input);

/* numbers */
sl_value sl_new_integer(int i);

#define NUM2INT(n) SL_INTEGER(n)->value

/* symbols */
sl_value sl_intern(struct sl_interpreter_state *state, char *name);

/* booleans */
extern sl_value sl_true;
extern sl_value sl_false;

/* lists */
extern sl_value sl_nil;

sl_value sl_size(sl_value list);
sl_value sl_first(sl_value list);
sl_value sl_rest(sl_value list);
