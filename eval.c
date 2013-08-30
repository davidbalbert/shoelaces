#include "shoelaces.h"

sl_value sl_symbol_table_get(struct sl_interpreter_state *state, char *name)
{
        khiter_t iter;

        iter = kh_get(str, state->symbol_table, name);

        if (iter == kh_end(state->symbol_table)) {
                return NULL;
        } else {
                return kh_value(state->symbol_table, iter);
        }
}

void sl_symbol_table_put(struct sl_interpreter_state *state, char *name, sl_value value)
{
        int ret;
        khiter_t iter;
        iter = kh_put(str, state->symbol_table, name, &ret);
        kh_value(state->symbol_table, iter) = value;
}

void sl_init_type();
void sl_init_symbol();
void sl_init_number();
void sl_init_boolean();
void sl_init_list();
void sl_init_string();
void sl_init_reader();
void sl_init_gc();
void sl_fix_type_names();

struct sl_interpreter_state *sl_init()
{
        struct sl_interpreter_state *state = sl_native_malloc(sizeof(struct sl_interpreter_state));
        state->symbol_table = kh_init(str);

        sl_init_gc();

        /* The order of these next three init calls is important.
         * Because type names are strings, and sl_tString isn't set
         * until after sl_init_string is called, the names of
         * sl_tType and sl_tString both have their types pointed at
         * something other than sl_tString. We fix this in
         * sl_fix_type_names.
         *
         * An alternative approach (that the Ruby interpreter
         * takes) is making type names Symbols instead of strings.
         * In the symbol table lookup function, the class of the
         * symbol name is checked before the symbol is returned. If
         * it's NULL, it is changed to the String class. The value
         * of doing things that way is that the order of the type
         * init functions would no longer matter and we could get
         * rid of sl_fix_type_names. I may experiment with doing
         * that at some point in the future. */
        sl_init_type();
        sl_init_string();
        sl_fix_type_names();

        sl_init_symbol();
        sl_init_number();
        sl_init_boolean();
        sl_init_list();

        sl_init_reader();

        return state;
}

void sl_destroy(struct sl_interpreter_state *state)
{
        kh_destroy(str, state->symbol_table);
        free(state);
}

/*
sl_value sl_eval(struct sl_interpreter_state *state, sl_value expression, sl_value environment)
{
        if (sl_type(expression) == sl_tSymbol) {
                if (sl_env_has_key(state, environment)
                        return sl_env_get(state, environment);
        } else {
                assert(0);
                return sl_false;
        }
}

sl_value sl_env_get(struct sl_interpreter_state *state, sl_value name)
{
        sl_value 
}
*/
