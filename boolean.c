#include "shoelaces.h"
#include "internal.h"

static
sl_value new_boolean(struct sl_interpreter_state *state, char value)
{
        sl_value b = sl_gc_alloc(state, sizeof(struct SLBoolean));
        SL_BASIC(b)->type = state->tBoolean;
        SL_BOOLEAN(b)->value = value;
        return b;
}

sl_value
sl_boolean_inspect(struct sl_interpreter_state *state, sl_value boolean)
{
        assert(sl_type(boolean) == state->tBoolean);
        if (SL_BOOLEAN(boolean)->value)
                return sl_string_new(state, "true");
        else
                return sl_string_new(state, "false");
}

void
sl_init_boolean(struct sl_interpreter_state *state)
{
        state->tBoolean = boot_type_new(state, sl_string_new(state, "Boolean"));
        state->sl_true = new_boolean(state, 1);
        state->sl_false = new_boolean(state, 0);
}
