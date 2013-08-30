#include "shoelaces.h"

sl_value sl_tBoolean;
sl_value sl_true;
sl_value sl_false;

static
sl_value new_boolean(struct sl_interpreter_state *state, char value)
{
        sl_value b = sl_gc_alloc(state, sizeof(struct SLBoolean));
        SL_BASIC(b)->type = sl_tBoolean;
        SL_BOOLEAN(b)->value = value;
        return b;
}

sl_value
sl_boolean_inspect(struct sl_interpreter_state *state, sl_value boolean)
{
        assert(sl_type(boolean) == sl_tBoolean);
        if (SL_BOOLEAN(boolean)->value)
                return sl_string_new(state, "true");
        else
                return sl_string_new(state, "false");
}

void
sl_init_boolean(struct sl_interpreter_state *state)
{
        sl_tBoolean = sl_type_new(state, sl_string_new(state, "Boolean"));
        sl_true = new_boolean(state, 1);
        sl_false = new_boolean(state, 0);
}
