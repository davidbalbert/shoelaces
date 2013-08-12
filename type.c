#include "shoelaces.h"

sl_value sl_Type;

sl_value sl_new_type(char *name)
{
        sl_value t = sl_alloc(struct SLType);
        SL_BASIC(t)->type = sl_Type;
        SL_TYPE(t)->name = name;
        return t;
}

void sl_init_type()
{
        sl_Type = sl_new_type("Type");
        SL_BASIC(sl_Type)->type = sl_Type;
}
