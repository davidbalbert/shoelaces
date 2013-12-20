#include <assert.h>
#include <stdio.h>

#include "shoelaces.h"
#include "internal.h"

static void add_c_method(struct sl_interpreter_state *state, sl_value func, sl_value signature, sl_value (*method_body)());
static void add_lisp_method(struct sl_interpreter_state *state, sl_value func, sl_value signature, sl_value bodies, sl_value environment);
static sl_value function_inspect(struct sl_interpreter_state *state, sl_value func);


static sl_value
method_table_new(struct sl_interpreter_state *state, sl_value function)
{
        assert(sl_type(function) == state->tFunction);

        sl_value method_table = sl_gc_alloc(state, sizeof(struct SLMethodTable));
        SL_BASIC(method_table)->type = state->tMethodTable;
        SL_METHOD_TABLE(method_table)->method_map = state->sl_empty_list;
        SL_METHOD_TABLE(method_table)->method = SLUndefined;
        SL_METHOD_TABLE(method_table)->varargs_method = SLUndefined;
        SL_METHOD_TABLE(method_table)->function = function;

        return method_table;
}

sl_value
func_new(struct sl_interpreter_state *state, sl_value name)
{
        assert(name == SLUndefined || sl_type(name) == state->tSymbol);

        sl_value f = sl_gc_alloc(state, sizeof(struct SLFunction));
        SL_BASIC(f)->type = state->tFunction;
        SL_FUNCTION(f)->name = name;
        SL_FUNCTION(f)->method_table = method_table_new(state, f);

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

sl_value
cfunc_varargs_invoker_1(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        return SL_METHOD(method)->cfunc_body(state, argv);
}

sl_value
cfunc_varargs_invoker_2(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        return SL_METHOD(method)->cfunc_body(state, sl_first(state, argv), sl_rest(state, argv));
}

sl_value
cfunc_varargs_invoker_3(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        sl_value first = sl_first(state, argv);
        sl_value second = sl_second(state, argv);
        sl_value rest = sl_rest(state, sl_rest(state, argv));

        return SL_METHOD(method)->cfunc_body(state, first, second, rest);
}

sl_value
cfunc_varargs_invoker_4(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        sl_value first = sl_first(state, argv);
        sl_value second = sl_second(state, argv);
        sl_value third = sl_third(state, argv);
        sl_value rest = sl_rest(state, sl_rest(state, sl_rest(state, argv)));

        return SL_METHOD(method)->cfunc_body(state, first, second, third, rest);
}

sl_value
cfunc_varargs_invoker_5(struct sl_interpreter_state *state, sl_value method, sl_value argv)
{
        assert(sl_type(method) == state->tMethod);
        assert(sl_type(argv) == state->tList); /* TODO: Make this an array */

        sl_value first = sl_first(state, argv);
        sl_value second = sl_second(state, argv);
        sl_value third = sl_third(state, argv);
        sl_value fourth = sl_fourth(state, argv);
        sl_value rest = sl_rest(state, sl_rest(state, sl_rest(state, sl_rest(state, argv))));

        return SL_METHOD(method)->cfunc_body(state, first, second, third, fourth, rest);
}

sl_cfunc_invoker
get_invoker_for_argc(int argc, int has_varargs)
{
        if (has_varargs) {
                switch (argc) {
                        case 1:
                                return cfunc_varargs_invoker_1;
                        case 2:
                                return cfunc_varargs_invoker_2;
                        case 3:
                                return cfunc_varargs_invoker_3;
                        case 4:
                                return cfunc_varargs_invoker_4;
                        case 5:
                                return cfunc_varargs_invoker_5;
                        default:
                                fprintf(stderr, "Error: %d isn't a supported cfunc argument count (0 to 5).\n", argc);
                                abort();
                }
        } else {
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
                                fprintf(stderr, "Error: %d isn't a supported cfunc argument count (0 to 5).\n", argc);
                                abort();
                }
        }
}

static int
signature_has_varargs(struct sl_interpreter_state *state, sl_value signature) {
        int ampersand_count = 0;
        sl_value list = signature;

        while (list != state->sl_empty_list) {
                if (sl_first(state, list) == state->s_ampersand) {
                        ampersand_count++;

                        if (ampersand_count > 1) {
                                fprintf(stderr, "Error: A method signature may not have more than one '&'\n");
                                abort();
                        }

                        if (NUM2INT(sl_list_size(state, list)) > 2) {
                                fprintf(stderr, "Error: '&' may only come before the last argument in a method signature\n");
                                abort();
                        }

                        if (NUM2INT(sl_list_size(state, list)) < 2) {
                                fprintf(stderr, "Error: '&' may not be the last argument in a method signature\n");
                                abort();
                        }

                }

                list = sl_rest(state, list);
        }

        return ampersand_count == 1;
}

static sl_value
method_new_lisp(struct sl_interpreter_state *state, sl_value function, sl_value signature, sl_value bodies, sl_value environment)
{
        assert(sl_type(function) == state->tFunction);
        assert(sl_type(signature) == state->tList);
        assert(sl_type(bodies) == state->tList);

        int arg_count = NUM2INT(sl_list_size(state, signature));

        int has_varargs = signature_has_varargs(state, signature);

        if (has_varargs) {
                arg_count--;
        }

        sl_value m = sl_gc_alloc(state, sizeof(struct SLMethod));
        SL_BASIC(m)->type = state->tMethod;
        SL_METHOD(m)->method_type = SL_METHOD_LISP;
        SL_METHOD(m)->signature = signature;
        SL_METHOD(m)->has_varargs = has_varargs;
        SL_METHOD(m)->function = function;
        SL_METHOD(m)->bodies = bodies;
        SL_METHOD(m)->environment = environment;

        return m;
}

static sl_value
method_new_cfunc(struct sl_interpreter_state *state, sl_value function, sl_value signature, sl_value (*method_body)())
{
        assert(sl_type(function) == state->tFunction);
        assert(sl_type(signature) == state->tList);

        int arg_count = NUM2INT(sl_list_size(state, signature));

        int has_varargs = signature_has_varargs(state, signature);

        if (has_varargs) {
                arg_count--;
        }

        sl_value m = sl_gc_alloc(state, sizeof(struct SLMethod));
        SL_BASIC(m)->type = state->tMethod;
        SL_METHOD(m)->method_type = SL_METHOD_CFUNC;
        SL_METHOD(m)->signature = signature;
        SL_METHOD(m)->has_varargs = has_varargs;
        SL_METHOD(m)->function = function;
        SL_METHOD(m)->cfunc_body = method_body;
        SL_METHOD(m)->invoker = get_invoker_for_argc(arg_count, has_varargs);

        return m;
}

static int
method_is_cfunc(struct sl_interpreter_state *state, sl_value method)
{
        assert(sl_type(method) == state->tMethod);

        return SL_METHOD(method)->method_type == SL_METHOD_CFUNC;
}

static sl_value
process_annotation(struct sl_interpreter_state *state, sl_value expr)
{
        /* Annotation expression. e.g (: Foo Bar) */
        if (sl_is_a_list(state, expr)) {
                assert(NUM2INT(sl_list_size(state, expr)) == 3);
                assert(sl_first(state, expr) == state->s_annotate);

                sl_value name = sl_second(state, expr);

                assert(sl_type(name) == state->tSymbol);

                if (sl_second(state, expr) == state->s_ampersand) {
                        fprintf(stderr, "Error: Special symbol `&' cannot be annotated in a method signature\n");
                        abort();
                }

                /* Eval here to call annotate-type if needed. This techincally
                 * supports arbitrary expression evaluation as long as it
                 * returns a type, but this isn't actually part of the
                 * language. It's just a side effect of the implementation.
                 * This will probably not be allowed once we have a compiler.
                 */
                sl_value type = sl_eval(state, sl_third(state, expr), state->global_env);

                assert(sl_type(type) == state->tType);

                return sl_symbol_annotate_with_type(state, name, type);
        } else {
                assert(sl_type(expr) == state->tSymbol);

                /* Don't annotate & */
                if (expr == state->s_ampersand) {
                        return expr;
                }

                return sl_symbol_annotate_with_type(state, expr, state->tAny);
        }
}

static sl_value
process_signature(struct sl_interpreter_state *state, sl_value signature)
{
        return sl_c_map(state, process_annotation, signature);
}

static sl_value
read_signature(struct sl_interpreter_state *state, char *signature)
{
        return process_signature(state, sl_read(state, signature));
}

sl_value
sl_fn(struct sl_interpreter_state *state, sl_value signature, sl_value bodies, sl_value environment)
{
        assert(sl_type(signature) == state->tList);
        assert(sl_type(bodies) == state->tList);

        sl_value func = func_new(state, SLUndefined);

        add_lisp_method(state, func, process_signature(state, signature), bodies, environment);

        return func;
}

void
sl_define_function(struct sl_interpreter_state *state, char *name, sl_value (*method_body)(), char *signature)
{
        sl_value func_name = sl_intern(state, name);
        sl_value func;

        if (sl_env_has_key(state, state->global_env, func_name)) {
                func = sl_env_get(state, state->global_env, func_name);

                if (sl_type(func) != state->tFunction) {
                        fprintf(stderr, "Error: `%s' is already defined but is not a function.\n", sl_string_cstring(state, sl_inspect(state, func_name)));
                        abort();
                }
        } else {
                func = func_new(state, func_name);
        }

        sl_value arg_list = read_signature(state, signature);

        add_c_method(state, func, arg_list, method_body);

        if (!sl_env_has_key(state, state->global_env, func_name)) {
                sl_def(state, func_name, func);
        }
}

static sl_value
method_table_for_signature(struct sl_interpreter_state *state, sl_value method_table, sl_value signature, sl_value func)
{
        if (signature == state->sl_empty_list) {
                return method_table;
        }

        /* Because of earlier checks, we can assume that there are zero or one
         * ampersands and that if there is one, it occurs in the second to last
         * position */
        if (sl_first(state, signature) == state->s_ampersand) {
                signature = sl_rest(state, signature);
        }

        sl_value next_arg_name = sl_first(state, signature);
        sl_value next_arg_type = sl_symbol_type_annotation(state, next_arg_name);

        sl_value method_map = SL_METHOD_TABLE(method_table)->method_map;
        sl_value rest_signature = sl_rest(state, signature);

        if (sl_alist_has_key(state, method_map, next_arg_type) == state->sl_true) {
                sl_value next_method_table = sl_alist_get(state, method_map, next_arg_type);
                return method_table_for_signature(state, next_method_table, rest_signature, func);
        } else {
                sl_value next_method_table = method_table_new(state, func);
                SL_METHOD_TABLE(method_table)->method_map = sl_alist_set(state, method_map, next_arg_type, next_method_table);
                return method_table_for_signature(state, next_method_table, rest_signature, func);
        }
}

static void
add_method_to_method_table(sl_value method, sl_value method_table)
{
        /* TODO: Warn on signature prefix + varargs ambiguity. E.g.:
         * (Int & Int)
         * (Int & String)
         *
         * requires
         *
         * (Int) to be defined */

        /* TODO: Warn and replace on this ambiguity:
         * (Int & Int)
         * (& Int)
         * */

        if (SL_METHOD(method)->has_varargs) {
                SL_METHOD_TABLE(method_table)->varargs_method = method;
        } else {
                SL_METHOD_TABLE(method_table)->method = method;
        }
}

static void
add_lisp_method(struct sl_interpreter_state *state, sl_value func, sl_value signature, sl_value bodies, sl_value environment)
{
        sl_value m = method_new_lisp(state, func, signature, bodies, environment);

        sl_value method_table = method_table_for_signature(state, SL_FUNCTION(func)->method_table, signature, func);

        add_method_to_method_table(m, method_table);
}

static void
add_c_method(struct sl_interpreter_state *state, sl_value func, sl_value signature, sl_value (*method_body)())
{
        sl_value m = method_new_cfunc(state, func, signature, method_body);

        sl_value method_table = method_table_for_signature(state, SL_FUNCTION(func)->method_table, signature, func);

        add_method_to_method_table(m, method_table);
}

static sl_value
function_name(struct sl_interpreter_state *state, sl_value func)
{
        assert(sl_type(func) == state->tFunction);

        if (SL_FUNCTION(func)->name) {
                return SL_FUNCTION(func)->name;
        } else {
                return sl_string_new(state, "(anonymous)");
        }
}

static sl_value
function_name_or_inspect(struct sl_interpreter_state *state, sl_value func)
{
        if (SL_FUNCTION(func)->name) {
                return SL_FUNCTION(func)->name;
        } else {
                return function_inspect(state, func);
        }
}

static sl_value
function_inspect(struct sl_interpreter_state *state, sl_value func)
{
        sl_value s = sl_string_new(state, "#<Function ");
        s = sl_string_concat(state, s, function_name(state, func));
        s = sl_string_concat(state, s, sl_string_new(state, ">"));
        return s;
}

static sl_value
method_list(struct sl_interpreter_state *state, sl_value method_table)
{
        struct SLMethodTable *mt = SL_METHOD_TABLE(method_table);
        sl_value ret = state->sl_empty_list;

        sl_value tables = sl_alist_values(state, mt->method_map);
        sl_value methods = state->sl_empty_list;

        for (sl_value t = tables; t != state->sl_empty_list; t = sl_rest(state, t)) {
                methods = sl_list_new(state, method_list(state, sl_first(state, t)), methods);
        }

        ret = sl_list_concat(state, methods);

        if (mt->method) {
                ret = sl_list_new(state, mt->method, ret);
        }

        if (mt->varargs_method) {
                ret = sl_list_new(state, mt->varargs_method, ret);
        }

        return ret;
}

static sl_value
method_table_inspect(struct sl_interpreter_state *state, sl_value method_table)
{
        assert(sl_type(method_table) == state->tMethodTable);
        struct SLMethodTable *mt = SL_METHOD_TABLE(method_table);

        sl_value methods = method_list(state, method_table);
        sl_value s = sl_string_new(state, "Methods for function \"");
        s = sl_string_concat(state, s, function_name_or_inspect(state, mt->function));
        s = sl_string_concat(state, s, sl_string_new(state, "\":\n"));
        s = sl_string_concat(state, s, sl_list_join(state, methods, sl_string_new(state, "\n")));

        return s;
}

static sl_value
arg_with_type(struct sl_interpreter_state *state, sl_value name)
{
        assert(sl_type(name) == state->tSymbol);

        if (name == state->s_ampersand) {
                return name;
        }

        assert(sl_symbol_type_annotation(state, name) != SLUndefined);

        sl_value s = sl_inspect(state, name);
        s = sl_string_concat(state, s, sl_string_new(state, ":"));
        s = sl_string_concat(state, s, sl_inspect(state, sl_symbol_type_annotation(state, name)));

        return s;
}

static sl_value
method_inspect(struct sl_interpreter_state *state, sl_value method)
{
        struct SLMethod *m = SL_METHOD(method);
        sl_value s = sl_string_new(state, "(");
        s = sl_string_concat(state, s, function_name_or_inspect(state, m->function));
        s = sl_string_concat(state, s, sl_string_new(state, " "));

        sl_value signature_with_types = sl_c_map(state, arg_with_type, m->signature);

        s = sl_string_concat(state, s, sl_list_join(state, signature_with_types, sl_string_new(state, " ")));
        s = sl_string_concat(state, s, sl_string_new(state, ")"));

        return s;
}

static int
is_value_subtype_of_type(sl_value val, sl_value type)
{
        return sl_is_subtype_of_type(sl_type(val), type);
}

static int
all_args_are_of_type(struct sl_interpreter_state *state, sl_value type, sl_value args) {
        sl_value args_left = args;

        while (sl_empty(state, args_left) == state->sl_false) {
                if (!is_value_subtype_of_type(sl_first(state, args_left), type)) {
                        return 0;
                }

                args_left = sl_rest(state, args_left);
        }

        return 1;
}

static sl_value
lookup_method(struct sl_interpreter_state *state, sl_value method_table, sl_value args)
{
        struct SLMethodTable *mt = SL_METHOD_TABLE(method_table);
        sl_value method = SLUndefined;
        sl_value next_method_table;

        if (args == state->sl_empty_list) {
                if (mt->method) {
                        return mt->method;
                } else if (mt->varargs_method) {
                        return mt->varargs_method;
                } else {
                        return SLUndefined;
                }
        }

        sl_value first_arg = sl_first(state, args);

        /* XXX: For matching against values. Essentially pattern matching. This
         * code path isn't used at the moment because there's no way to define
         * a function in terms of values, only types */
        /*
        if (sl_alist_has_key(state, mt->method_map, first_arg) == state->sl_true) {
                next_method_table = sl_alist_get(state, mt->method_map, first_arg);
                method = lookup_method(state, next_method_table, sl_rest(state, args));

                if (method != SLUndefined) {
                        return method;
                }
        }
        */

        sl_value type = sl_type(first_arg);

        while (type != SLUndefined) {
                if (sl_alist_has_key(state, mt->method_map, type) == state->sl_true) {
                        next_method_table = sl_alist_get(state, mt->method_map, type);
                        method = lookup_method(state, next_method_table, sl_rest(state, args));

                        if (method != SLUndefined) {
                                return method;
                        } else if (all_args_are_of_type(state, type, args) && SL_METHOD_TABLE(next_method_table)->varargs_method) {
                                return SL_METHOD_TABLE(next_method_table)->varargs_method;
                        }
                }

                type = SL_TYPE(type)->super;
        }

        return SLUndefined;
}

static sl_value
method_for_arguments(struct sl_interpreter_state *state, sl_value func, sl_value args)
{
        sl_value method = lookup_method(state, SL_FUNCTION(func)->method_table, args);

        if (method == SLUndefined) {
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
                struct sl_keep_list *old = state->keep_list;

                sl_value val = SL_METHOD(method)->invoker(state, method, args);

                sl_free_keep_list(state->keep_list, old);
                state->keep_list = old;

                return val;
        } else {
                sl_value env = SL_METHOD(method)->environment;
                sl_value bodies = SL_METHOD(method)->bodies;

                sl_value out = SLUndefined;

                while (bodies != state->sl_empty_list) {
                        out = sl_eval(state, sl_first(state, bodies), env);
                        bodies = sl_rest(state, bodies);
                }

                return out;
        }
}

static sl_value
methods(struct sl_interpreter_state *state, sl_value func)
{
        assert(sl_type(func) == state->tFunction);

        return SL_FUNCTION(func)->method_table;
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

        sl_define_function(state, "inspect", function_inspect, "(f:Function)");
        sl_define_function(state, "inspect", method_inspect, "(m:Method)");
        sl_define_function(state, "inspect", method_table_inspect, "(mt:MethodTable)");

        sl_define_function(state, "methods", methods, "(f:Function)");
}
