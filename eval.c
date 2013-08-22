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

struct sl_interpreter_state *sl_init()
{
        struct sl_interpreter_state *state = malloc(sizeof(struct sl_interpreter_state));
        state->symbol_table = kh_init(str);

        sl_init_gc();

        sl_init_type();
        sl_init_symbol();
        sl_init_number();
        sl_init_boolean();
        sl_init_list();
        sl_init_string();

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
