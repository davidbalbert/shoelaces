#include "shoelaces.h"

sl_value sl_symbol_table_get(struct sl_interpreter_state *state, char *name)
{
        int ret;
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

        sl_init_type();
        sl_init_symbol();
        sl_init_number();
        sl_init_boolean();

        return state;
}

void sl_destroy(struct sl_interpreter_state *state)
{
        kh_destroy(str, state->symbol_table);
        free(state);
}
