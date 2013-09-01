#include "shoelaces.h"
#include "internal.h"

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

sl_value
sl_intern_string(struct sl_interpreter_state *state, sl_value string)
{
        assert(sl_type(string) == state->tString);

        return sl_intern(state, sl_string_cstring(state, string));
}

static
sl_value new_symbol(struct sl_interpreter_state *state, sl_value name)
{
        sl_value sym = sl_gc_alloc(state, sizeof(struct SLSymbol));
        SL_BASIC(sym)->type = state->tSymbol;
        SL_SYMBOL(sym)->name = name;
        return sym;
}

sl_value
sl_symbol_inspect(struct sl_interpreter_state *state, sl_value symbol)
{
        assert(sl_type(symbol) == state->tSymbol);
        return SL_SYMBOL(symbol)->name;
}

void
sl_init_symbol(struct sl_interpreter_state *state)
{
        state->tSymbol = boot_type_new(state, sl_string_new(state, "Symbol"));

        state->s_def = sl_intern(state, "def");
        state->s_quote = sl_intern(state, "quote");
}
