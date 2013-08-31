#include "shoelaces.h"
#include "internal.h"

sl_value
sl_integer_new(struct sl_interpreter_state *state, int i)
{
        sl_value n = sl_gc_alloc(state, sizeof(struct SLInteger));
        SL_BASIC(n)->type = state->tInteger;
        SL_INTEGER(n)->value = i;
        return n;
}

sl_value
sl_integer_inspect(struct sl_interpreter_state *state, sl_value integer)
{
        char inspect_str[CHAR_BIT * sizeof(long)];

        assert(sl_type(integer) == state->tInteger);

        sprintf(inspect_str, "%ld", NUM2INT(integer));

        return sl_string_new(state, inspect_str);
}

long
NUM2INT(sl_value n)
{
        return SL_INTEGER(n)->value;
}

void
sl_init_number(struct sl_interpreter_state *state)
{
        state->tInteger = boot_type_new(state, sl_string_new(state, "Integer"));
}
