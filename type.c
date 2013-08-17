#include "shoelaces.h"

sl_value sl_tType;

sl_value sl_new_type(char *name)
{
        sl_value t = sl_alloc(struct SLType);
        SL_BASIC(t)->type = sl_tType;
        SL_TYPE(t)->name = name;
        return t;
}

sl_value sl_type(sl_value object)
{
        if (object == sl_nil)
        {
                return sl_tList;
        } else {
                return SL_BASIC(object)->type;
        }
}

void sl_init_type()
{
        sl_tType = sl_new_type("Type");
        SL_BASIC(sl_tType)->type = sl_tType;
}
