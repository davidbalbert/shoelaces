#include "shoelaces.h"

sl_value sl_tList;
sl_value sl_nil;

sl_value sl_new_list(sl_value first, sl_value rest)
{
        sl_value l = sl_alloc(struct SLList);
        SL_BASIC(l)->type = sl_tList;
        SL_LIST(l)->first = first;
        SL_LIST(l)->rest = rest;
        return l;
}

void sl_init_list()
{
        sl_tList = sl_new_type("List");
        sl_nil = sl_new_list(NULL, NULL);
}
