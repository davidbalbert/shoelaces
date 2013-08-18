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

sl_value sl_list_inspect(sl_value list)
{
        sl_value str;

        if (sl_empty(list) == sl_true) {
                return sl_new_string("()");
        } else {
                str = sl_new_string("(");
                str = sl_string_concat(str, sl_list_join(list, sl_new_string(" ")));
                return sl_string_concat(str, sl_new_string(")"));
        }
}

sl_value sl_list_join(sl_value strings, sl_value seperator)
{
        sl_value output = sl_new_string("");

        /* TODO: tagged falsy values would be nice */
        while (sl_empty(strings) != sl_true) {
                output = sl_string_concat(output, sl_inspect(sl_first(strings)));
                strings = sl_rest(strings);

                /* TODO: tagged falsey values */
                if (sl_empty(strings) != sl_true)
                        output = sl_string_concat(output, seperator);
        }

        return output;
}

static sl_value sl_new_empty_list()
{
        return sl_alloc_list(NULL, NULL, sl_new_integer(0));
}

sl_value sl_new_list(sl_value first, sl_value rest)
{
        sl_value new_size;

        if (sl_type(rest) == sl_tList) {
                new_size = sl_new_integer(NUM2INT(sl_size(rest)) + 1);
        } else {
                /* TODO: The size of a list shouldn't be false. We
                 * should raise an exception here instead because
                 * you're not allowed to call size on a dotted pair
                 * */
                new_size = sl_false;
        }

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

sl_value sl_size(sl_value val)
{
        sl_value type = sl_type(val);
        assert(type == sl_tList || type == sl_tString);

        if (type == sl_tList)
                return SL_LIST(val)->size;
        else
                return SL_STRING(val)->size;
}

sl_value sl_reverse(sl_value list)
{
        assert(sl_type(list) == sl_tList);

        sl_value new_list = sl_empty_list;

        while (sl_empty(list) != sl_true) {
                new_list = sl_new_list(sl_first(list), new_list);
                list = sl_rest(list);
        }

        return new_list;
}

sl_value sl_empty(sl_value list)
{
        if (list == sl_empty_list)
                return sl_true;
        else
                return sl_false;
}

void sl_init_list()
{
        sl_tList = sl_new_type(sl_new_string("List"));
        sl_empty_list = sl_new_empty_list();
}
