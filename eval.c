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
        int ret;
        khiter_t iter;
        iter = kh_put(str, state->symbol_table, name, &ret);
        kh_value(state->symbol_table, iter) = value;
}

size_t
sl_symbol_table_size(struct sl_interpreter_state *state)
{
        return kh_size(state->symbol_table);
}

void sl_init_type(struct sl_interpreter_state *state);
void sl_init_symbol(struct sl_interpreter_state *state);
void sl_init_number(struct sl_interpreter_state *state);
void sl_init_boolean(struct sl_interpreter_state *state);
void sl_init_list(struct sl_interpreter_state *state);
void sl_init_string(struct sl_interpreter_state *state);
void sl_init_reader(struct sl_interpreter_state *state);
void sl_init_gc(struct sl_interpreter_state *state);
void sl_fix_type_names(struct sl_interpreter_state *state);

struct sl_interpreter_state *
sl_init()
{
        struct sl_interpreter_state *state = sl_native_malloc(sizeof(struct sl_interpreter_state));
        state->symbol_table = kh_init(str);

        sl_init_gc(state);

        /* The order of these next three function calls is
         * important.  Because type names are strings, and
         * sl_tString isn't set until after sl_init_string is
         * called, the names of sl_tType and sl_tString both have
         * their types pointed at something other than sl_tString.
         * We fix this in sl_fix_type_names.
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
        sl_init_type(state);
        sl_init_string(state);
        sl_fix_type_names(state);

        sl_init_list(state);
        sl_init_symbol(state);
        sl_init_number(state);
        sl_init_boolean(state);

        sl_init_reader(state);

        state->global_env = state->sl_empty_list;

        boot_def_type(state, state->tType);
        boot_def_type(state, state->tString);
        boot_def_type(state, state->tList);
        boot_def_type(state, state->tSymbol);
        boot_def_type(state, state->tInteger);
        boot_def_type(state, state->tBoolean);

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
sl_eval(struct sl_interpreter_state *state, sl_value expression, sl_value environment)
{
        if (sl_type(expression) == state->tSymbol) {
                if (env_has_key(state, environment, expression)) {
                        return env_get(state, environment, expression);
                } else {
                        fprintf(stderr, "Error: `%s' is undefined\n", sl_string_cstring(state, sl_inspect(state, expression)));
                        abort();
                }
        } else if (sl_type(expression) == state->tString ||
                   sl_type(expression) == state->tInteger ||
                   sl_type(expression) == state->tBoolean) {
                return expression;
        } else if (sl_type(expression) == state->tList &&
                   sl_type(sl_first(state, expression)) == state->tSymbol) {
                sl_value first = sl_first(state, expression);

                if (state->s_def == first) {
                        assert(NUM2INT(sl_size(state, expression)) == 3);

                        sl_value second = sl_second(state, expression);
                        sl_value third = sl_third(state, expression);
                        return sl_def(state, second, sl_eval(state, third, environment));
                } else if (state->s_quote == first) {
                        assert(NUM2INT(sl_size(state, expression)) == 2);

                        return sl_second(state, expression);
                } else if (state->s_if == first) {
                        assert(NUM2INT(sl_size(state, expression)) == 4);

                        sl_value rest = sl_rest(state, expression);
                        sl_value result = sl_eval(state, sl_first(state, rest), environment);

                        if (result == state->sl_false) {
                                return sl_eval(state, sl_third(state, rest), environment);
                        } else {
                                return sl_eval(state, sl_second(state, rest), environment);
                        }
                } else {
                        fprintf(stderr, "Error: `%s' is not implemented yet\n", sl_string_cstring(state, sl_inspect(state, expression)));
                        abort();
                }
        } else {
                fprintf(stderr, "Error: `%s' is not implemented yet\n", sl_string_cstring(state, sl_inspect(state, expression)));
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
        }

        return state->sl_false;
}
