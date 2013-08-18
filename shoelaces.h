#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

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

struct SLString
{
        struct SLBasic basic;
        char *value;
        sl_value size;
};

#define SL_BASIC(v)   ((struct SLBasic*)(v))
#define SL_TYPE(v)    ((struct SLType*)(v))
#define SL_SYMBOL(v)  ((struct SLSymbol*)(v))
#define SL_INTEGER(v) ((struct SLInteger*)(v))
#define SL_BOOLEAN(v) ((struct SLBoolean*)(v))
#define SL_LIST(v)    ((struct SLList*)(v))
#define SL_STRING(v)  ((struct SLString*)(v))

extern sl_value sl_tType;
extern sl_value sl_tSymbol;
extern sl_value sl_tInteger;
extern sl_value sl_tBoolean;
extern sl_value sl_tList;
extern sl_value sl_tString;

sl_value sl_new_type(char *name);
sl_value sl_type(sl_value object);

/* printing */
void sl_type_p(sl_value type);
void sl_integer_p(sl_value integer);
void sl_symbol_p(sl_value symbol);
void sl_boolean_p(sl_value boolean);
void sl_list_p(sl_value list);
void sl_string_p(sl_value string);

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
void sl_init_string();
void sl_init_reader();

sl_value sl_symbol_table_get(struct sl_interpreter_state *state, char *name);
void sl_symbol_table_put(struct sl_interpreter_state *state, char *name, sl_value value);

/* util functions */
void *memzero(void *p, size_t length);

/* gc - haha */
#define sl_alloc(type) (type*)malloc(sizeof(type))

/* io */
void sl_p(sl_value val);

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
extern sl_value sl_empty_list;

sl_value sl_new_list(sl_value first, sl_value rest);
sl_value sl_size(sl_value list);
sl_value sl_first(sl_value list);
sl_value sl_rest(sl_value list);
sl_value sl_reverse(sl_value list);

/* strings */
char *sl_string_cstring(sl_value string);

sl_value sl_new_string(char *value);
