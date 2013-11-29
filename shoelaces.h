#include "khash.h"

/* types */
typedef uintptr_t sl_value;

#define SLUndefined 0

struct SLBasic
{
        sl_value type;
        unsigned int gc_mark:1;
};

struct SLType
{
        struct SLBasic basic;

        /* TODO: Change to Symbol? If you do this, you must also
         * change fix_type_names */
        sl_value name; /* String */
        sl_value super; /* Type */
        sl_value parameters; /* List */
        unsigned int abstract:1;
};

struct SLSymbol
{
        struct SLBasic basic;
        sl_value name; /* String */
};

struct SLKeyword
{
        struct SLBasic basic;
        sl_value name; /* String */
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
        sl_value first; /* Any */
        sl_value rest; /* Any */
        size_t size;
};

struct SLString
{
        struct SLBasic basic;
        char *value;
        size_t size;
};

struct SLFunction
{
        struct SLBasic basic;
        sl_value name; /* Symbol */
        sl_value method_table; /* MethodTable */
};

struct SLMethodTable
{
        struct SLBasic basic;
        sl_value method; /* Method */
        sl_value varargs_method; /* Method */
        sl_value method_map; /* Association List */
        sl_value function; /* Function */
};

struct sl_interpreter_state;

typedef sl_value (*sl_cfunc_invoker)(struct sl_interpreter_state *state, sl_value method, sl_value argv);

struct SLMethod
{
        struct SLBasic basic;
        sl_value signature; /* List */
        sl_value function; /* Function */
        int has_varargs;

        enum {
                SL_METHOD_CFUNC,
                SL_METHOD_LISP
        } method_type;

        /* For C functions */
        sl_value (*cfunc_body)();
        sl_cfunc_invoker invoker;

        /* For LISP functions */
        sl_value bodies;
        sl_value environment;
};

#define SL_BASIC(v)         ((struct SLBasic *)(v))
#define SL_TYPE(v)          ((struct SLType *)(v))
#define SL_SYMBOL(v)        ((struct SLSymbol *)(v))
#define SL_KEYWORD(v)       ((struct SLKeyword *)(v))
#define SL_INTEGER(v)       ((struct SLInteger *)(v))
#define SL_BOOLEAN(v)       ((struct SLBoolean *)(v))
#define SL_LIST(v)          ((struct SLList *)(v))
#define SL_STRING(v)        ((struct SLString *)(v))
#define SL_FUNCTION(v)      ((struct SLFunction *)(v))
#define SL_METHOD_TABLE(v)  ((struct SLMethodTable *)(v))
#define SL_METHOD(v)        ((struct SLMethod *)(v))

/* interpreter state and setup */
KHASH_MAP_INIT_STR(str, sl_value);

struct sl_keep_list {
        sl_value first;
        struct sl_keep_list *rest;
};

struct sl_interpreter_state {
        khash_t(str) *symbol_table;
        khash_t(str) *keyword_table;

        struct sl_heap *heap;
        struct sl_keep_list *keep_list; /* temp variables on the C stack */

        sl_value global_env;

        sl_value tType;
        sl_value tAny;
        sl_value tNone;
        sl_value tSymbol;
        sl_value tInteger;
        sl_value tBoolean;
        sl_value tList;
        sl_value tString;
        sl_value tFunction;
        sl_value tMethodTable;
        sl_value tMethod;
        sl_value tKeyword;

        sl_value sl_true;
        sl_value sl_false;
        sl_value sl_empty_list;

        sl_value s_def;
        sl_value s_quote;
        sl_value s_if;
        sl_value s_annotate;
        sl_value s_ampersand;
        sl_value s_fn;

        int gc_enabled;
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
sl_value sl_abstract_type_new(struct sl_interpreter_state *state, sl_value name);
sl_value sl_type(sl_value object);
sl_value sl_apply_type(struct sl_interpreter_state *state, sl_value abstract_type, sl_value type_parameters);
sl_value sl_types(struct sl_interpreter_state *state, sl_value values);

/* equality */
sl_value sl_equal(struct sl_interpreter_state *state, sl_value a, sl_value b);

/* eval */
sl_value sl_eval(struct sl_interpreter_state *state, sl_value expression, sl_value environment);
sl_value sl_def(struct sl_interpreter_state *state, sl_value name, sl_value value);

/* functions */
void sl_define_function(struct sl_interpreter_state *state, char *name, sl_value (*method_body)(), sl_value type_list);
sl_value sl_apply(struct sl_interpreter_state *state, sl_value func, sl_value args);

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

/* keywords */
sl_value sl_intern_keyword(struct sl_interpreter_state *state, char *name);

/* lists */

sl_value sl_list_new(struct sl_interpreter_state *state, sl_value first, sl_value rest);
sl_value sl_list(struct sl_interpreter_state *state, size_t size, ...);
sl_value sl_list_size(struct sl_interpreter_state *state, sl_value list);
sl_value sl_first(struct sl_interpreter_state *state, sl_value list);
sl_value sl_second(struct sl_interpreter_state *state, sl_value list);
sl_value sl_third(struct sl_interpreter_state *state, sl_value list);
sl_value sl_fourth(struct sl_interpreter_state *state, sl_value list);
sl_value sl_fifth(struct sl_interpreter_state *state, sl_value list);
sl_value sl_rest(struct sl_interpreter_state *state, sl_value list);
sl_value sl_reverse(struct sl_interpreter_state *state, sl_value list);
sl_value sl_empty(struct sl_interpreter_state *state, sl_value list);
sl_value sl_list_join(struct sl_interpreter_state *state, sl_value strings, sl_value seperator);
sl_value sl_list_contains(struct sl_interpreter_state *state, sl_value list, sl_value item);
sl_value sl_list_concat(struct sl_interpreter_state *state, sl_value lists);

/* association lists */

sl_value sl_alist_has_key(struct sl_interpreter_state *state, sl_value alist, sl_value key);
sl_value sl_alist_get(struct sl_interpreter_state *state, sl_value alist, sl_value key);
sl_value sl_alist_set(struct sl_interpreter_state *state, sl_value alist, sl_value key, sl_value value);
sl_value sl_alist_values(struct sl_interpreter_state *state, sl_value alist);

/* strings */
char *sl_string_cstring(struct sl_interpreter_state *state, sl_value string);

sl_value sl_string_new(struct sl_interpreter_state *state, char *value);
sl_value sl_inspect(struct sl_interpreter_state *state, sl_value val);
sl_value sl_string_concat(struct sl_interpreter_state *state, sl_value s1, sl_value s2);
sl_value sl_string_size(struct sl_interpreter_state *state, sl_value string);
sl_value sl_string_empty(struct sl_interpreter_state *state, sl_value string);
