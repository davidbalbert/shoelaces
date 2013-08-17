#include "shoelaces.h"

sl_value sl_tInteger;

sl_value sl_new_integer(int i)
{
        sl_value n = sl_alloc(struct SLInteger);
        SL_BASIC(n)->type = sl_tInteger;
        SL_INTEGER(n)->value = i;
        return n;
}

void sl_integer_p(sl_value integer)
{
        assert(sl_type(integer) == sl_tInteger);
        printf("%ld\n", NUM2INT(integer));
}

void sl_init_number()
{
        sl_tInteger = sl_new_type("Integer");
}
