#include "shoelaces.h"

sl_value sl_tList;
sl_value sl_empty_list;

static sl_value
sl_alloc_list(struct sl_interpreter_state *state, sl_value first, sl_value rest, size_t size)
{
        sl_value l = sl_gc_alloc(state, sizeof(struct SLList));
        SL_BASIC(l)->type = sl_tList;
        SL_LIST(l)->first = first;
        SL_LIST(l)->rest = rest;
        SL_LIST(l)->size = size;
        return l;
}

sl_value
sl_list_inspect(struct sl_interpreter_state *state, sl_value list)
{
        sl_value str;

        if (sl_empty(list) == sl_true) {
                return sl_string_new(state, "()");
        } else {
                str = sl_string_new(state, "(");
                str = sl_string_concat(state, str, sl_list_join(state, list, sl_string_new(state, " ")));
                return sl_string_concat(state, str, sl_string_new(state, ")"));
        }
}

sl_value
sl_list_join(struct sl_interpreter_state *state, sl_value strings, sl_value seperator)
{
        sl_value output = sl_string_new(state, "");

        /* TODO: tagged falsy values would be nice */
        while (sl_empty(strings) != sl_true) {
                output = sl_string_concat(state, output, sl_inspect(state, sl_first(strings)));
                strings = sl_rest(strings);

                /* TODO: tagged falsey values */
                if (sl_empty(strings) != sl_true)
                        output = sl_string_concat(state, output, seperator);
        }

        return output;
}

static
sl_value sl_empty_list_new(struct sl_interpreter_state *state)
{
        /* TODO: these NULL's should move to undefineds when sl_false becomes 0 */
        return sl_alloc_list(state, NULL, NULL, 0);
}

sl_value
sl_list_new(struct sl_interpreter_state *state, sl_value first, sl_value rest)
{
        size_t new_size;

        if (sl_type(rest) == sl_tList) {
                new_size = NUM2INT(sl_size(state, rest)) + 1;
        } else {
                /* TODO: The size of a list should never be -1. We
                 * should raise an exception here instead because
                 * you're not allowed to call size on a dotted pair
                 * */
                new_size = -1;
        }

        return sl_alloc_list(state, first, rest, new_size);
}

sl_value
sl_first(sl_value list)
{
        assert(sl_type(list) == sl_tList);
        return SL_LIST(list)->first;
}

sl_value
sl_rest(sl_value list)
{
        assert(sl_type(list) == sl_tList);
        return SL_LIST(list)->rest;
}

sl_value
sl_size(struct sl_interpreter_state *state, sl_value val)
{
        sl_value type = sl_type(val);
        assert(type == sl_tList || type == sl_tString);

        if (type == sl_tList)
                return sl_integer_new(state, SL_LIST(val)->size);
        else
                return sl_integer_new(state, SL_STRING(val)->size);
}

sl_value
sl_reverse(struct sl_interpreter_state *state, sl_value list)
{
        assert(sl_type(list) == sl_tList);

        sl_value new_list = sl_empty_list;

        while (sl_empty(list) != sl_true) {
                new_list = sl_list_new(state, sl_first(list), new_list);
                list = sl_rest(list);
        }

        return new_list;
}

sl_value
sl_empty(sl_value list)
{
        if (list == sl_empty_list)
                return sl_true;
        else
                return sl_false;
}

void
sl_init_list(struct sl_interpreter_state *state)
{
        sl_tList = sl_type_new(state, sl_string_new(state, "List"));
        sl_empty_list = sl_empty_list_new(state);
}
