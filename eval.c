#include "shoelaces.h"
#include "internal.h"

sl_value
sl_symbol_table_get(struct sl_interpreter_state *state, char *name)
{
        khiter_t iter;

        iter = kh_get(str, state->symbol_table, name);

        if (iter == kh_end(state->symbol_table)) {
                return NULL;
        } else {
                return kh_value(state->symbol_table, iter);
        }
}

void
sl_symbol_table_put(struct sl_interpreter_state *state, char *name, sl_value value)
{
        assert(sl_type(value) == state->tSymbol);

        /* TODO: Check for ENOMEM */
        char *duped_name = strdup(name);
        int ret;
        khiter_t iter;
        iter = kh_put(str, state->symbol_table, duped_name, &ret);
        kh_value(state->symbol_table, iter) = value;
}

size_t
sl_symbol_table_size(struct sl_interpreter_state *state)
{
        return kh_size(state->symbol_table);
}

void boot_type(struct sl_interpreter_state *state);
void boot_string(struct sl_interpreter_state *state);
void fix_type_names(struct sl_interpreter_state *state);
void boot_list(struct sl_interpreter_state *state);
void boot_symbol(struct sl_interpreter_state *state);
void boot_keyword(struct sl_interpreter_state *state);
void boot_boolean(struct sl_interpreter_state *state);

void sl_init_type(struct sl_interpreter_state *state);
void sl_init_string(struct sl_interpreter_state *state);
void sl_init_list(struct sl_interpreter_state *state);
void sl_init_symbol(struct sl_interpreter_state *state);
void sl_init_keyword(struct sl_interpreter_state *state);
void sl_init_number(struct sl_interpreter_state *state);
void sl_init_boolean(struct sl_interpreter_state *state);
void sl_init_function(struct sl_interpreter_state *state);
void sl_init_eval(struct sl_interpreter_state *state);
void sl_init_gc(struct sl_interpreter_state *state);
void sl_init_reader(struct sl_interpreter_state *state);

struct sl_interpreter_state *
sl_init()
{
        struct sl_interpreter_state *state = sl_native_malloc(sizeof(struct sl_interpreter_state));

        state->symbol_table = kh_init(str);
        state->keyword_table = kh_init(str);

        sl_init_gc(state);

        /* The order of these next three function calls is
         * important.  Because type names are strings, and
         * state->tString isn't set until after boot_string is
         * called, the names of state->tType and state->tString
         * both have their types pointed at something other than
         * state->tString.  We fix this in fix_type_names.
         *
         * An alternative approach (that the Ruby interpreter
         * takes) is making type names Symbols instead of strings.
         * In the symbol table lookup function, the class of the
         * symbol name is checked before the symbol is returned. If
         * it's NULL, it is changed to the String class. The value
         * of doing things that way is that the order of the type
         * init functions would no longer matter and we could get
         * rid of sl_fix_type_names. I may experiment with doing
         * that at some point in the future.
         */

        /* bootstrap types necessary for interpreter interals */
        boot_type(state);
        boot_string(state);
        fix_type_names(state);

        boot_list(state);
        boot_symbol(state);
        boot_keyword(state);
        boot_boolean(state);

        /* set up global environment and add existing types to it */
        state->global_env = state->sl_empty_list;

        boot_def_type(state, state->tType);
        boot_def_type(state, state->tAny);
        boot_def_type(state, state->tString);
        boot_def_type(state, state->tList);
        boot_def_type(state, state->tSymbol);
        boot_def_type(state, state->tKeyword);
        boot_def_type(state, state->tBoolean);

        /* initialize functions (the rest of the initialization methods depend on this */
        sl_init_function(state);

        /* finish initialization of bootstrapped types */
        sl_init_type(state);
        sl_init_string(state);
        sl_init_list(state);
        sl_init_symbol(state);
        sl_init_keyword(state);
        sl_init_boolean(state);

        /* initialize other types here */
        sl_init_number(state);
        sl_init_eval(state);

        /* initialize the reader */
        sl_init_reader(state);

        return state;
}

void
sl_destroy(struct sl_interpreter_state *state)
{
        kh_destroy(str, state->symbol_table);
        sl_gc_free_all(state);
        free(state);
}

int
env_has_key(struct sl_interpreter_state *state, sl_value environment, sl_value name)
{
        return sl_alist_has_key(state, environment, name) == state->sl_true;
}

sl_value
env_get(struct sl_interpreter_state *state, sl_value environment, sl_value name)
{
        return sl_alist_get(state, environment, name);
}

sl_value
eval_each(struct sl_interpreter_state *state, sl_value args, sl_value environment)
{
        assert(sl_type(args) == state->tList);

        if (args == state->sl_empty_list) {
                return state->sl_empty_list;
        } else {
                sl_value first = sl_first(state, args);
                sl_value rest = sl_rest(state, args);
                return sl_list_new(state, sl_eval(state, first, environment), eval_each(state, rest, environment));
        }
}

sl_value
sl_eval(struct sl_interpreter_state *state, sl_value expression, sl_value environment)
{
        if (sl_type(expression) == state->tSymbol) {
                if (env_has_key(state, environment, expression)) {
                        return env_get(state, environment, expression);
                } else {
                        fprintf(stderr, "Error: `%s' is undefined\n", sl_string_cstring(state, sl_inspect(state, expression)));
                        abort();
                }
        } else if (sl_type(expression) != state->tList) {
                return expression;
        } else if (sl_empty(state, expression) == state->sl_true) {
                return expression;
        } else if (sl_type(sl_first(state, expression)) == state->tSymbol) {
                sl_value first = sl_first(state, expression);

                if (state->s_def == first) {
                        assert(NUM2INT(sl_list_size(state, expression)) == 3);

                        sl_value second = sl_second(state, expression);
                        sl_value third = sl_third(state, expression);
                        return sl_def(state, second, sl_eval(state, third, environment));
                } else if (state->s_quote == first) {
                        assert(NUM2INT(sl_list_size(state, expression)) == 2);

                        return sl_second(state, expression);
                } else if (state->s_if == first) {
                        assert(NUM2INT(sl_list_size(state, expression)) == 4);

                        sl_value rest = sl_rest(state, expression);
                        sl_value result = sl_eval(state, sl_first(state, rest), environment);

                        if (result == state->sl_false) {
                                return sl_eval(state, sl_third(state, rest), environment);
                        } else {
                                return sl_eval(state, sl_second(state, rest), environment);
                        }
                } else if (state->s_annotate == first) {
                        assert(NUM2INT(sl_list_size(state, expression)) == 3);

                        sl_value val = sl_eval(state, sl_second(state, expression), environment);
                        sl_value type = sl_eval(state, sl_third(state, expression), environment);

                        assert(sl_type(val) == type);

                        return val;
                } else {
                        sl_value f = sl_eval(state, first, environment);
                        sl_value new_expression = sl_list_new(state, f, sl_rest(state, expression));

                        return sl_eval(state, new_expression, environment);
                }
        } else if (sl_type(sl_first(state, expression)) == state->tFunction) {
                sl_value f = sl_first(state, expression);
                sl_value args = sl_rest(state, expression);

                /* TODO: make eval_each a map and eval */
                return sl_apply(state, f, eval_each(state, args, environment));
        } else {
                fprintf(stderr, "Error: %s is not implemented yet\n", sl_string_cstring(state, sl_inspect(state, expression)));
                abort();
        }
}

sl_value
sl_def(struct sl_interpreter_state *state, sl_value name, sl_value value)
{
        assert(sl_type(name) == state->tSymbol);
        state->global_env = sl_alist_set(state, state->global_env, name, value);
        return value;
}

sl_value
sl_eq(struct sl_interpreter_state *state, sl_value a, sl_value b)
{
        return a == b ? state->sl_true : state->sl_false;
}

sl_value
sl_equals(struct sl_interpreter_state *state, sl_value a, sl_value b) {
        if (a == b) {
                return state->sl_true;
        }

        if(sl_type(a) != sl_type(b)) {
                return state->sl_false;
        }

        sl_value type = sl_type(a);

        if (type == state->tString) {
                int result = strcmp(sl_string_cstring(state, a), sl_string_cstring(state, b));
                return result == 0 ? state->sl_true : state->sl_false;
        } else if (type == state->tInteger) {
                return NUM2INT(a) == NUM2INT(b) ? state->sl_true : state->sl_false;
        } else if (type == state->tList) {
                if (NUM2INT(sl_list_size(state, a)) != NUM2INT(sl_list_size(state, b))) {
                        return state->sl_false;
                }

                sl_value first_a = sl_first(state, a);
                sl_value first_b = sl_first(state, b);

                if (sl_equals(state, first_a, first_b) == state->sl_true) {
                        sl_value rest_a = sl_rest(state, a);
                        sl_value rest_b = sl_rest(state, b);

                        return sl_equals(state, rest_a, rest_b);
                } else {
                        return state->sl_false;
                }
        }

        return state->sl_false;
}

void
sl_init_eval(struct sl_interpreter_state *state)
{
        sl_define_function(state, "eq?", sl_eq, sl_list(state, 2, state->tAny, state->tAny));
        sl_define_function(state, "equals?", sl_equals, sl_list(state, 2, state->tAny, state->tAny));
}
