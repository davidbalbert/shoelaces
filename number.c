#include "shoelaces.h"

sl_value sl_Integer;

sl_value sl_new_integer(int i)
{
        sl_value n = sl_alloc(struct SLInteger);
        SL_BASIC(n)->type = sl_Integer;
        SL_INTEGER(n)->value = i;
        return n;
}

void sl_init_number()
{
        sl_Integer = sl_new_type("Integer");
}
