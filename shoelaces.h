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
        unsigned int gc_mark:1;
};

struct SLType
{
        struct SLBasic basic;
        sl_value name;
};

struct SLSymbol
{
        struct SLBasic basic;
        sl_value name;
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
        sl_value size; // Maybe this should be an int
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

sl_value sl_type_new(sl_value name);
sl_value sl_type(sl_value object);

/* inspection */
sl_value sl_type_inspect(sl_value type);
sl_value sl_integer_inspect(sl_value integer);
sl_value sl_symbol_inspect(sl_value symbol);
sl_value sl_boolean_inspect(sl_value boolean);
sl_value sl_list_inspect(sl_value list);
sl_value sl_string_inspect(sl_value string);

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
void sl_init_gc();

sl_value sl_symbol_table_get(struct sl_interpreter_state *state, char *name);
void sl_symbol_table_put(struct sl_interpreter_state *state, char *name, sl_value value);

/* util functions */
void *memzero(void *p, size_t length);

/* eval */
sl_value sl_eval(struct sl_interpreter_state *state, sl_value expression, sl_value environment);

/* variable bindings */
sl_value sl_env_get(struct sl_interpreter_state *state, sl_value name);

/* gc - haha */

struct sl_heap
{
        sl_value *slots;
        size_t capacity;
        size_t size;
};

size_t sl_gc_heap_size();
void sl_gc_run();
sl_value sl_gc_alloc(size_t size);

#define sl_alloc(type) sl_gc_alloc(sizeof(type))
#define sl_dealloc(value) free(value)

/* io */
void sl_p(sl_value val);

/* reader */
sl_value sl_read(struct sl_interpreter_state *state, char *input);

/* numbers */
sl_value sl_integer_new(int i);

#define NUM2INT(n) SL_INTEGER(n)->value

/* symbols */
sl_value sl_intern(struct sl_interpreter_state *state, char *name);

/* booleans */
extern sl_value sl_true;
extern sl_value sl_false;

/* lists */
extern sl_value sl_empty_list;

sl_value sl_list_new(sl_value first, sl_value rest);
sl_value sl_size(sl_value list);
sl_value sl_first(sl_value list);
sl_value sl_rest(sl_value list);
sl_value sl_reverse(sl_value list);
sl_value sl_empty(sl_value list);
sl_value sl_list_join(sl_value strings, sl_value seperator);

/* strings */
char *sl_string_cstring(sl_value string);

sl_value sl_string_new(char *value);
sl_value sl_inspect(sl_value val);
sl_value sl_string_concat(sl_value s1, sl_value s2);
