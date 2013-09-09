#include "shoelaces.h"
#include "internal.h"

static void add_method(struct sl_interpreter_state *state, sl_value func, sl_value (*method_body)(), int arg_count, sl_value type_list);

static sl_value
method_table_new(struct sl_interpreter_state *state)
{
        sl_value method_table = sl_gc_alloc(state, sizeof(struct SLMethodTable));
        SL_BASIC(method_table)->type = state->tMethodTable;
        SL_METHOD_TABLE(method_table)->method_map = state->sl_empty_list;
        SL_METHOD_TABLE(method_table)->method = NULL;

        return method_table;
}

sl_value
func_new(struct sl_interpreter_state *state, sl_value name)
{
        assert(sl_type(name) == state->tSymbol);

        sl_value f = sl_gc_alloc(state, sizeof(struct SLFunction));
        SL_BASIC(f)->type = state->tFunction;
        SL_FUNCTION(f)->name = name;
        SL_FUNCTION(f)->method_table = method_table_new(state);

        return f;
}

sl_value
cfunc_invoker_0(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        return SL_METHOD(method)->cfunc_body(state);
}

sl_value
cfunc_invoker_1(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        return SL_METHOD(method)->cfunc_body(state, sl_first(state, argv));
}

sl_value
cfunc_invoker_2(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        sl_value first = sl_first(state, argv);
        sl_value second = sl_second(state, argv);

        return SL_METHOD(method)->cfunc_body(state, first, second);
}

sl_value
cfunc_invoker_3(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        sl_value first = sl_first(state, argv);
        sl_value second = sl_second(state, argv);
        sl_value third = sl_third(state, argv);

        return SL_METHOD(method)->cfunc_body(state, first, second, third);
}

sl_value
cfunc_invoker_4(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        sl_value first = sl_first(state, argv);
        sl_value second = sl_second(state, argv);
        sl_value third = sl_third(state, argv);
        sl_value fourth = sl_fourth(state, argv);

        return SL_METHOD(method)->cfunc_body(state, first, second, third, fourth);
}

sl_value
cfunc_invoker_5(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        sl_value first = sl_first(state, argv);
        sl_value second = sl_second(state, argv);
        sl_value third = sl_third(state, argv);
        sl_value fourth = sl_fourth(state, argv);
        sl_value fifth = sl_fifth(state, argv);

        return SL_METHOD(method)->cfunc_body(state, first, second, third, fourth, fifth);
}

sl_cfunc_invoker
get_invoker_for_argc(int argc)
{
        switch (argc) {
                case 0:
                        return cfunc_invoker_0;
                case 1:
                        return cfunc_invoker_1;
                case 2:
                        return cfunc_invoker_2;
                case 3:
                        return cfunc_invoker_3;
                case 4:
                        return cfunc_invoker_4;
                case 5:
                        return cfunc_invoker_5;
                default:
                        fprintf(stderr, "Error: %d is an supported cfunc argument count (0 to 5).\n", argc);
                        abort();
        }
}

sl_value
method_new_cfunc(struct sl_interpreter_state *state, sl_value function, sl_value (*method_body)(), int arg_count, sl_value signature)
{
        assert(sl_type(function) == state->tFunction);
        assert(sl_type(signature) == state->tList);

        /* TODO: figure out argcount for vararg methods */
        assert(NUM2INT(sl_list_size(state, signature)) == arg_count);

        sl_value m = sl_gc_alloc(state, sizeof(struct SLMethod));
        SL_BASIC(m)->type = state->tMethod;
        SL_METHOD(m)->method_type = SL_METHOD_CFUNC;
        SL_METHOD(m)->signature = signature;
        SL_METHOD(m)->function = function;
        SL_METHOD(m)->cfunc_body = method_body;
        SL_METHOD(m)->invoker = get_invoker_for_argc(arg_count);

        return m;
}

int
method_is_cfunc(struct sl_interpreter_state *state, sl_value method)
{
        assert(sl_type(method) == state->tMethod);

        return SL_METHOD(method)->method_type == SL_METHOD_CFUNC;
}

void
sl_define_function(struct sl_interpreter_state *state, char *name, sl_value (*method_body)(), int arg_count, sl_value type_list)
{
        sl_value func_name = sl_intern(state, name);
        sl_value func;

        if (env_has_key(state, state->global_env, func_name)) {
                func = env_get(state, state->global_env, func_name);

                if (sl_type(func) != state->tFunction) {
                        fprintf(stderr, "Error: `%s' is already defined but is not a function.\n", sl_string_cstring(state, sl_inspect(state, func_name)));
                        abort();
                }
        } else {
                func = func_new(state, func_name);
        }

        add_method(state, func, method_body, arg_count, type_list);

        if (!env_has_key(state, state->global_env, func_name)) {
                sl_def(state, func_name, func);
        }
}

static sl_value
method_table_for_signature(struct sl_interpreter_state *state, sl_value method_table, sl_value signature)
{
        if (signature == state->sl_empty_list) {
                return method_table;
        }

        sl_value next_arg = sl_first(state, signature);
        sl_value method_map = SL_METHOD_TABLE(method_table)->method_map;
        sl_value rest_signature = sl_rest(state, signature);

        if (sl_alist_has_key(state, method_map, next_arg) == state->sl_true) {
                sl_value next_method_table = sl_alist_get(state, method_map, next_arg);
                return method_table_for_signature(state, next_method_table, rest_signature);
        } else {
                sl_value next_method_table = method_table_new(state);
                SL_METHOD_TABLE(method_table)->method_map = sl_alist_set(state, method_map, next_arg, next_method_table);
                return method_table_for_signature(state, next_method_table, rest_signature);
        }
}

static void
add_method(struct sl_interpreter_state *state, sl_value func, sl_value (*method_body)(), int arg_count, sl_value signature)
{
        sl_value m = method_new_cfunc(state, func, method_body, arg_count, signature);

        sl_value method_table = method_table_for_signature(state, SL_FUNCTION(func)->method_table, signature);

        SL_METHOD_TABLE(method_table)->method = m;
}

sl_value
sl_function_inspect(struct sl_interpreter_state *state, sl_value func)
{
        if (SL_FUNCTION(func)->name == NULL) {
                return sl_string_new(state, "#<Function (anonymous)>");
        } else {
                sl_value s = sl_string_new(state, "#<Function ");
                s = sl_string_concat(state, s, sl_inspect(state, SL_FUNCTION(func)->name));
                s = sl_string_concat(state, s, sl_string_new(state, ">"));
                return s;
        }
}

sl_value
sl_method_table_inspect(struct sl_interpreter_state *state, sl_value method_table)
{
        struct SLMethodTable *mt = SL_METHOD_TABLE(method_table);
        sl_value s = sl_string_new(state, "#<MethodTable method: ");

        if (mt->method != NULL) {
                s = sl_string_concat(state, s, sl_inspect(state, mt->method));
        } else {
                s = sl_string_concat(state, s, sl_string_new(state, "NULL"));
        }

        s = sl_string_concat(state, s, sl_string_new(state, " method_map: "));
        s = sl_string_concat(state, s, sl_inspect(state, mt->method_map));

        return s;
}

sl_value
sl_method_inspect(struct sl_interpreter_state *state, sl_value method)
{
        struct SLMethod *m = SL_METHOD(method);
        sl_value s = sl_string_new(state, "#<Method ");
        s = sl_string_concat(state, s, sl_inspect(state, SL_FUNCTION(m->function)->name));
        s = sl_string_concat(state, s, sl_string_new(state, " "));
        s = sl_string_concat(state, s, sl_inspect(state, m->signature));

        return s;
}

static sl_value
lookup_method(struct sl_interpreter_state *state, sl_value method_table, sl_value args)
{
        struct SLMethodTable *mt = SL_METHOD_TABLE(method_table);
        sl_value method = NULL;
        sl_value next_method_table;

        if (args == state->sl_empty_list) {
                if (mt->method) {
                        return mt->method;
                } else {
                        return NULL;
                }
        }

        sl_value first_arg = sl_first(state, args);

        if (sl_alist_has_key(state, mt->method_map, first_arg) == state->sl_true) {
                next_method_table = sl_alist_get(state, mt->method_map, first_arg);
                method = lookup_method(state, next_method_table, sl_rest(state, args));

                if (method != NULL) {
                        return method;
                }
        }

        sl_value type = sl_type(first_arg);

        while (type != NULL) {
                if (sl_alist_has_key(state, mt->method_map, type) == state->sl_true) {
                        next_method_table = sl_alist_get(state, mt->method_map, type);
                        method = lookup_method(state, next_method_table, sl_rest(state, args));

                        if (method != NULL) {
                                return method;
                        }
                }

                type = SL_TYPE(type)->super;
        }

        return NULL;
}

static sl_value
method_for_arguments(struct sl_interpreter_state *state, sl_value func, sl_value args)
{
        sl_value method = lookup_method(state, SL_FUNCTION(func)->method_table, args);

        if (method == NULL) {
                /* TODO: replace sl_types with "map" and "type" */
                sl_value types = sl_types(state, args);

                fprintf(stderr, "Error: %s is not defined for %s\n",
                                sl_string_cstring(state, sl_inspect(state, func)),
                                sl_string_cstring(state, sl_inspect(state, types)));
                abort();
        }

        return method;
}

sl_value
sl_apply(struct sl_interpreter_state *state, sl_value func, sl_value args)
{
        assert(sl_type(func) == state->tFunction);
        assert(sl_type(args) == state->tList);

        sl_value method = method_for_arguments(state, func, args);

        if (method_is_cfunc(state, method)) {
                return SL_METHOD(method)->invoker(state, method, args);
        } else {
                fprintf(stderr, "Error: only cfuncs are supported at the moment.\n");
                abort();
        }
}

void
sl_init_function(struct sl_interpreter_state *state)
{
        state->tFunction = sl_type_new(state, sl_string_new(state, "Function"));
        state->tMethod = sl_type_new(state, sl_string_new(state, "Method"));

        /* I don't love that the MethodTable type is exposed as it's really an
         * internal type. Maybe we'll do something about that, but for now, we
         * need it in the environment so that we can mark it. */
        state->tMethodTable = sl_type_new(state, sl_string_new(state, "MethodTable"));
}
