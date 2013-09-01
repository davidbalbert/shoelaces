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

/* interpreter state and setup */
KHASH_MAP_INIT_STR(str, sl_value);

struct sl_interpreter_state {
        khash_t(str) *symbol_table;
        struct sl_heap *heap;

        sl_value global_env;

        sl_value tType;
        sl_value tSymbol;
        sl_value tInteger;
        sl_value tBoolean;
        sl_value tList;
        sl_value tString;

        sl_value sl_true;
        sl_value sl_false;
        sl_value sl_empty_list;
};

struct sl_interpreter_state *sl_init();
void sl_destroy(struct sl_interpreter_state *state);

sl_value sl_symbol_table_get(struct sl_interpreter_state *state, char *name);
void sl_symbol_table_put(struct sl_interpreter_state *state, char *name, sl_value value);
size_t sl_symbol_table_size(struct sl_interpreter_state *state);

/* util functions */
void * memzero(void *p, size_t length);

/* more types */
sl_value sl_type_new(struct sl_interpreter_state *state, sl_value name);
sl_value sl_type(sl_value object);

/* inspection */
sl_value sl_type_inspect(struct sl_interpreter_state *state, sl_value type);
sl_value sl_integer_inspect(struct sl_interpreter_state *state, sl_value integer);
sl_value sl_symbol_inspect(struct sl_interpreter_state *state, sl_value symbol);
sl_value sl_boolean_inspect(struct sl_interpreter_state *state, sl_value boolean);
sl_value sl_list_inspect(struct sl_interpreter_state *state, sl_value list);
sl_value sl_string_inspect(struct sl_interpreter_state *state, sl_value string);

/* equality */
sl_value sl_equals(struct sl_interpreter_state *state, sl_value a, sl_value b);

/* eval */
sl_value sl_eval(struct sl_interpreter_state *state, sl_value expression, sl_value environment);
sl_value sl_def(struct sl_interpreter_state *state, sl_value name, sl_value value);

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
void sl_dealloc(struct sl_interpreter_state *state, sl_value value);

/* io */
void sl_p(struct sl_interpreter_state *state, sl_value val);

/* reader */
sl_value sl_read(struct sl_interpreter_state *state, char *input);

/* numbers */
sl_value sl_integer_new(struct sl_interpreter_state *state, int i);
long NUM2INT(sl_value n);

/* symbols */
sl_value sl_intern(struct sl_interpreter_state *state, char *name);
sl_value sl_intern_string(struct sl_interpreter_state *state, sl_value string);

/* lists */

sl_value sl_list_new(struct sl_interpreter_state *state, sl_value first, sl_value rest);
sl_value sl_size(struct sl_interpreter_state *state, sl_value list);
sl_value sl_first(struct sl_interpreter_state *state, sl_value list);
sl_value sl_second(struct sl_interpreter_state *state, sl_value list);
sl_value sl_third(struct sl_interpreter_state *state, sl_value list);
sl_value sl_rest(struct sl_interpreter_state *state, sl_value list);
sl_value sl_reverse(struct sl_interpreter_state *state, sl_value list);
sl_value sl_empty(struct sl_interpreter_state *state, sl_value list);
sl_value sl_list_join(struct sl_interpreter_state *state, sl_value strings, sl_value seperator);

/* association lists */

sl_value sl_alist_has_key(struct sl_interpreter_state *state, sl_value alist, sl_value key);
sl_value sl_alist_get(struct sl_interpreter_state *state, sl_value alist, sl_value key);
sl_value sl_alist_set(struct sl_interpreter_state *state, sl_value alist, sl_value key, sl_value value);

/* strings */
char *sl_string_cstring(struct sl_interpreter_state *state, sl_value string);

sl_value sl_string_new(struct sl_interpreter_state *state, char *value);
sl_value sl_inspect(struct sl_interpreter_state *state, sl_value val);
sl_value sl_string_concat(struct sl_interpreter_state *state, sl_value s1, sl_value s2);
