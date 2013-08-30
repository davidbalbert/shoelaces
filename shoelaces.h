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
        size_t size;
};

struct SLString
{
        struct SLBasic basic;
        char *value;
        size_t size;
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

/* interpreter state and setup */
KHASH_MAP_INIT_STR(str, sl_value);

struct sl_interpreter_state {
        khash_t(str) *symbol_table;
        struct sl_heap *heap;
};

struct sl_interpreter_state *sl_init();
void sl_destroy(struct sl_interpreter_state *state);

sl_value sl_symbol_table_get(struct sl_interpreter_state *state, char *name);
void sl_symbol_table_put(struct sl_interpreter_state *state, char *name, sl_value value);

/* util functions */
void * memzero(void *p, size_t length);



sl_value sl_type_new(struct sl_interpreter_state *state, sl_value name);
sl_value sl_type(sl_value object);

/* inspection */
sl_value sl_type_inspect(struct sl_interpreter_state *state, sl_value type);
sl_value sl_integer_inspect(struct sl_interpreter_state *state, sl_value integer);
sl_value sl_symbol_inspect(struct sl_interpreter_state *state, sl_value symbol);
sl_value sl_boolean_inspect(struct sl_interpreter_state *state, sl_value boolean);
sl_value sl_list_inspect(struct sl_interpreter_state *state, sl_value list);
sl_value sl_string_inspect(struct sl_interpreter_state *state, sl_value string);

/* eval */
sl_value sl_eval(struct sl_interpreter_state *state, sl_value expression, sl_value environment);

/* variable bindings */
sl_value sl_env_get(struct sl_interpreter_state *state, sl_value name);

/* gc */

struct sl_heap
{
        sl_value *slots;
        size_t capacity;
        size_t size;
};

size_t sl_gc_heap_size(struct sl_interpreter_state *state);
void sl_gc_run(struct sl_interpreter_state *state);
sl_value sl_gc_alloc(struct sl_interpreter_state *state, size_t size);
void * sl_native_malloc(size_t size);

#define sl_dealloc(value) free(value)

/* io */
void sl_p(struct sl_interpreter_state *state, sl_value val);

/* reader */
sl_value sl_read(struct sl_interpreter_state *state, char *input);

/* numbers */
sl_value sl_integer_new(struct sl_interpreter_state *state, int i);

#define NUM2INT(n) SL_INTEGER(n)->value

/* symbols */
sl_value sl_intern(struct sl_interpreter_state *state, char *name);

/* booleans */
extern sl_value sl_true;
extern sl_value sl_false;

/* lists */
extern sl_value sl_empty_list;

sl_value sl_list_new(struct sl_interpreter_state *state, sl_value first, sl_value rest);
sl_value sl_size(struct sl_interpreter_state *state, sl_value list);
sl_value sl_first(sl_value list);
sl_value sl_rest(sl_value list);
sl_value sl_reverse(struct sl_interpreter_state *state, sl_value list);
sl_value sl_empty(sl_value list);
sl_value sl_list_join(struct sl_interpreter_state *state, sl_value strings, sl_value seperator);

/* strings */
char *sl_string_cstring(sl_value string);

sl_value sl_string_new(struct sl_interpreter_state *state, char *value);
sl_value sl_inspect(struct sl_interpreter_state *state, sl_value val);
sl_value sl_string_concat(struct sl_interpreter_state *state, sl_value s1, sl_value s2);
