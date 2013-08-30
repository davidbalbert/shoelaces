#include "shoelaces.h"

sl_value sl_tSymbol;

static sl_value new_symbol(struct sl_interpreter_state *state, sl_value name);

sl_value
sl_intern(struct sl_interpreter_state *state, char *name)
{
        sl_value sym;
        if ((sym = sl_symbol_table_get(state, name))) {
                return sym;
        } else {
                sym = new_symbol(state, sl_string_new(state, name));
                sl_symbol_table_put(state, name, sym);
                return sym;
        }
}

static
sl_value new_symbol(struct sl_interpreter_state *state, sl_value name)
{
        sl_value sym = sl_gc_alloc(state, sizeof(struct SLSymbol));
        SL_BASIC(sym)->type = sl_tSymbol;
        SL_SYMBOL(sym)->name = name;
        return sym;
}

sl_value
sl_symbol_inspect(struct sl_interpreter_state *state, sl_value symbol)
{
        assert(sl_type(symbol) == sl_tSymbol);
        return SL_SYMBOL(symbol)->name;
}

void
sl_init_symbol(struct sl_interpreter_state *state)
{
        sl_tSymbol = sl_type_new(state, sl_string_new(state, "Symbol"));
}
