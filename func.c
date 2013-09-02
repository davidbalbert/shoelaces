#include "shoelaces.h"
#include "internal.h"

void add_method(struct sl_interpreter_state *state, sl_value func, sl_value (*method_body)(), int arg_count, sl_value type_list);

sl_value
func_new(struct sl_interpreter_state *state, sl_value name)
{
        assert(sl_type(name) == state->tSymbol);

        sl_value f = sl_gc_alloc(state, sizeof(struct SLFunction));
        SL_BASIC(f)->type = state->tFunction;
        SL_FUNCTION(f)->name = name;
        SL_FUNCTION(f)->methods = state->sl_empty_list;

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
method_new_cfunc(struct sl_interpreter_state *state, sl_value function, sl_value (*method_body)(), int arg_count, sl_value type_list)
{
        assert(sl_type(function) == state->tFunction);

        /* TODO: Make type_list an array. Scaning it on invoke is order N*M :( */
        assert(sl_type(type_list) == state->tList);
        assert(NUM2INT(sl_size(state, type_list)) == arg_count);

        sl_value m = sl_gc_alloc(state, sizeof(struct SLMethod));
        SL_BASIC(m)->type = state->tMethod;
        SL_METHOD(m)->method_type = SL_METHOD_CFUNC;
        SL_METHOD(m)->signature = type_list;
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

void
add_method(struct sl_interpreter_state *state, sl_value func, sl_value (*method_body)(), int arg_count, sl_value type_list)
{
        sl_value m = method_new_cfunc(state, func, method_body, arg_count, type_list);

        SL_FUNCTION(func)->methods = sl_alist_set(state, SL_FUNCTION(func)->methods, type_list, m);
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
sl_apply(struct sl_interpreter_state *state, sl_value func, sl_value args)
{
        assert(sl_type(func) == state->tFunction);
        assert(sl_type(args) == state->tList);

        /* TODO: replace sl_types with "map" and "type" */
        sl_value types = sl_types(state, args);

        if (sl_alist_has_key(state, SL_FUNCTION(func)->methods, types) == state->sl_false) {
                fprintf(stderr, "Error: %s is not defined for %s\n",
                                sl_string_cstring(state, sl_inspect(state, func)),
                                sl_string_cstring(state, sl_inspect(state, types)));
                abort();
        }

        sl_value method = sl_alist_get(state, SL_FUNCTION(func)->methods, types);

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
}
