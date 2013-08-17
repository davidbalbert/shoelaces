#include "shoelaces.h"

sl_value sl_tBoolean;
sl_value sl_true;
sl_value sl_false;

static sl_value sl_new_boolean(char value)
{
        sl_value b = sl_alloc(struct SLBoolean);
        SL_BASIC(b)->type = sl_tBoolean;
        SL_BOOLEAN(b)->value = value;
        return b;
}

void sl_boolean_p(sl_value boolean)
{
        assert(sl_type(boolean) == sl_tBoolean);
        if (SL_BOOLEAN(boolean)->value)
                printf("true\n");
        else
                printf("false\n");
}

void sl_init_boolean()
{
        sl_tBoolean = sl_new_type("Boolean");
        sl_true = sl_new_boolean(1);
        sl_false = sl_new_boolean(0);
}
