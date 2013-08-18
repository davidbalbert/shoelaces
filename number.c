#include "shoelaces.h"

sl_value sl_tInteger;

sl_value sl_new_integer(int i)
{
        sl_value n = sl_alloc(struct SLInteger);
        SL_BASIC(n)->type = sl_tInteger;
        SL_INTEGER(n)->value = i;
        return n;
}

sl_value sl_integer_inspect(sl_value integer)
{
        char inspect_str[CHAR_BIT * sizeof(long)];

        assert(sl_type(integer) == sl_tInteger);

        sprintf(inspect_str, "%ld", NUM2INT(integer));

        return sl_new_string(inspect_str);
}

void sl_init_number()
{
        sl_tInteger = sl_new_type(sl_new_string("Integer"));
}
