#include <stdio.h>
#include <assert.h>

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

static sl_value
integer_inspect(struct sl_interpreter_state *state, sl_value integer)
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

static sl_value
integer_add_2(struct sl_interpreter_state *state, sl_value a, sl_value b)
{
        assert(sl_type(a) == state->tInteger);
        assert(sl_type(b) == state->tInteger);

        return sl_integer_new(state, NUM2INT(a) + NUM2INT(b));
}

void
sl_init_number(struct sl_interpreter_state *state)
{
        state->tInteger = sl_type_new(state, sl_string_new(state, "Integer"));

        sl_define_function(state, "inspect", integer_inspect, "(i:Integer)");
        //sl_define_function(state, "+", integer_add_2, sl_list(state, 2, state->tInteger, state->tInteger));
}
