#include "shoelaces.h"

sl_value sl_tBoolean;
sl_value sl_true;
sl_value sl_false;

static sl_value sl_boolean_new(char value)
{
        sl_value b = sl_alloc(struct SLBoolean);
        SL_BASIC(b)->type = sl_tBoolean;
        SL_BOOLEAN(b)->value = value;
        return b;
}

sl_value sl_boolean_inspect(sl_value boolean)
{
        assert(sl_type(boolean) == sl_tBoolean);
        if (SL_BOOLEAN(boolean)->value)
                return sl_string_new("true");
        else
                return sl_string_new("false");
}

void sl_init_boolean()
{
        sl_tBoolean = sl_type_new(sl_string_new("Boolean"));
        sl_true = sl_boolean_new(1);
        sl_false = sl_boolean_new(0);
}
