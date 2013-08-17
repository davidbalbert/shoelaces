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
        return SL_BASIC(object)->type;
}

void sl_type_p(sl_value type)
{
        assert(sl_type(type) == sl_tType);
        printf("%s\n", SL_TYPE(type)->name);
}

void sl_init_type()
{
        sl_tType = sl_new_type("Type");
        SL_BASIC(sl_tType)->type = sl_tType;
}
