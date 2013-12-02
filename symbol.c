#include <assert.h>

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

static sl_value
new_symbol(struct sl_interpreter_state *state, sl_value name)
{
        sl_value sym = sl_gc_alloc(state, sizeof(struct SLSymbol));
        SL_BASIC(sym)->type = state->tSymbol;
        SL_SYMBOL(sym)->name = name;
        SL_SYMBOL(sym)->type_annotation = SLUndefined;
        return sym;
}

sl_value
sl_symbol_annotate_with_type(struct sl_interpreter_state *state, sl_value sym, sl_value type)
{
        assert(sl_type(sym) == state->tSymbol);
        assert(sl_type(type) == state->tType);

        sl_value new_sym = new_symbol(state, SL_SYMBOL(sym)->name);
        SL_SYMBOL(new_sym)->type_annotation = type;

        return new_sym;
}

sl_value
sl_symbol_type_annotation(struct sl_interpreter_state *state, sl_value sym)
{
        assert(sl_type(sym) == state->tSymbol);
        assert(SL_SYMBOL(sym)->type_annotation != SLUndefined);

        return SL_SYMBOL(sym)->type_annotation;
}

static sl_value
symbol_inspect(struct sl_interpreter_state *state, sl_value symbol)
{
        assert(sl_type(symbol) == state->tSymbol);
        return SL_SYMBOL(symbol)->name;
}


/* NOTE: This function is needed so that all required types can be set up
 * before we start adding things to the environment. If in doubt, you should
 * put your Symbol initialization code into sl_init_symbol. */

/* NOTE: Don't call sl_define_function in boot_symbol. Sl_define_function
 * expects the environment to be set up. */
void
boot_symbol(struct sl_interpreter_state *state)
{
        state->tSymbol = boot_type_new(state, sl_string_new(state, "Symbol"));

        state->s_def = sl_intern(state, "def");
        state->s_quote = sl_intern(state, "quote");
        state->s_if = sl_intern(state, "if");
        state->s_annotate = sl_intern(state, ":");
        state->s_ampersand = sl_intern(state, "&");
        state->s_fn = sl_intern(state, "fn");
}

void
sl_init_symbol(struct sl_interpreter_state *state)
{
        //sl_define_function(state, "intern", sl_intern_string, sl_list(state, 1, state->tString));
        sl_define_function(state, "inspect", symbol_inspect, "(sym:Symbol)");
}
