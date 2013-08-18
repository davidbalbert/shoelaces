#include "shoelaces.h"

sl_value sl_tSymbol;

static sl_value sl_new_symbol(sl_value name);

sl_value sl_intern(struct sl_interpreter_state *state, char *name)
{
        sl_value sym;
        if ((sym = sl_symbol_table_get(state, name))) {
                return sym;
        } else {
                sym = sl_new_symbol(sl_new_string(name));
                sl_symbol_table_put(state, name, sym);
                return sym;
        }
}

static sl_value sl_new_symbol(sl_value name)
{
        sl_value sym = sl_alloc(struct SLSymbol);
        SL_BASIC(sym)->type = sl_tSymbol;
        SL_SYMBOL(sym)->name = name;
        return sym;
}

sl_value sl_symbol_inspect(sl_value symbol)
{
        assert(sl_type(symbol) == sl_tSymbol);
        return SL_SYMBOL(symbol)->name;
}

void sl_init_symbol()
{
        sl_tSymbol = sl_new_type(sl_new_string("Symbol"));
}
