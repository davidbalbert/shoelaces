#include "shoelaces.h"

sl_value
sl_type_new(struct sl_interpreter_state *state, sl_value name)
{
        sl_value t = sl_gc_alloc(state, sizeof(struct SLType));
        SL_BASIC(t)->type = state->tType;
        SL_TYPE(t)->name = name;
        return t;
}

sl_value
sl_type(sl_value object)
{
        return SL_BASIC(object)->type;
}

sl_value
sl_type_inspect(struct sl_interpreter_state *state, sl_value type)
{
        assert(sl_type(type) == state->tType);

        return SL_TYPE(type)->name;
}

void
sl_fix_type_names(struct sl_interpreter_state *state)
{
        SL_BASIC(SL_TYPE(state->tType)->name)->type = state->tString;
        SL_BASIC(SL_TYPE(state->tString)->name)->type = state->tString;
}

void
sl_init_type(struct sl_interpreter_state *state)
{
        state->tType = sl_type_new(state, sl_string_new(state, "Type"));
        SL_BASIC(state->tType)->type = state->tType;
}
