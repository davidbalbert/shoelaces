#include <assert.h>

#include "shoelaces.h"

sl_value sl_tList;
sl_value sl_nil;

sl_value sl_new_list(sl_value first, sl_value rest, sl_value size)
{
        sl_value l = sl_alloc(struct SLList);
        SL_BASIC(l)->type = sl_tList;
        SL_LIST(l)->first = first;
        SL_LIST(l)->rest = rest;
        SL_LIST(l)->size = size;
        return l;
}

static sl_value sl_new_empty_list()
{
        return sl_new_list(NULL, NULL, sl_new_integer(0));
}

sl_value sl_first(sl_value list)
{
        assert(sl_type(list) == sl_tList);
        return SL_LIST(list)->first;
}

sl_value sl_rest(sl_value list)
{
        assert(sl_type(list) == sl_tList);
        return SL_LIST(list)->rest;
}

sl_value sl_size(sl_value list)
{
        assert(sl_type(list) == sl_tList);
        return SL_LIST(list)->size;
}

void sl_init_list()
{
        sl_tList = sl_new_type("List");
        sl_nil = sl_new_empty_list();
}
