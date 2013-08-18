#include "shoelaces.h"

sl_value sl_tList;
sl_value sl_empty_list;

static sl_value sl_alloc_list(sl_value first, sl_value rest, sl_value size)
{
        sl_value l = sl_alloc(struct SLList);
        SL_BASIC(l)->type = sl_tList;
        SL_LIST(l)->first = first;
        SL_LIST(l)->rest = rest;
        SL_LIST(l)->size = size;
        return l;
}

void sl_list_p(sl_value list)
{
        sl_value l = list;

        if (l == sl_empty_list) {
                printf("nil");
        } else {
                printf("(");

                while (l != sl_empty_list) {
                        sl_p(sl_first(l));
                        l = sl_rest(l);
                }

                printf(")");
        }

}



static sl_value sl_new_empty_list()
{
        return sl_alloc_list(NULL, NULL, sl_new_integer(0));
}

sl_value sl_new_list(sl_value first, sl_value rest)
{
        sl_value new_size = sl_new_integer(NUM2INT(sl_size(rest)) + 1);
        return sl_alloc_list(first, rest, new_size);
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

sl_value sl_reverse(sl_value list)
{
        assert(sl_type(list) == sl_tList);

        sl_value new_list = sl_empty_list;

        while (list != sl_empty_list) {
                new_list = sl_new_list(sl_first(list), new_list);
                list = sl_rest(list);
        }

        return new_list;
}

void sl_init_list()
{
        sl_tList = sl_new_type("List");
        sl_empty_list = sl_new_empty_list();
}
