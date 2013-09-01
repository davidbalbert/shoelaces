#include "shoelaces.h"
#include "internal.h"

static sl_value
sl_alloc_list(struct sl_interpreter_state *state, sl_value first, sl_value rest, size_t size)
{
        sl_value l = sl_gc_alloc(state, sizeof(struct SLList));
        SL_BASIC(l)->type = state->tList;
        SL_LIST(l)->first = first;
        SL_LIST(l)->rest = rest;
        SL_LIST(l)->size = size;
        return l;
}

sl_value
sl_list_inspect(struct sl_interpreter_state *state, sl_value list)
{
        sl_value str;

        if (sl_empty(state, list) == state->sl_true) {
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
        while (sl_empty(state, strings) != state->sl_true) {
                output = sl_string_concat(state, output, sl_inspect(state, sl_first(state, strings)));
                strings = sl_rest(state, strings);

                /* TODO: tagged falsey values */
                if (sl_empty(state, strings) != state->sl_true)
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

        if (sl_type(rest) == state->tList) {
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
sl_first(struct sl_interpreter_state *state, sl_value list)
{
        assert(sl_type(list) == state->tList);
        return SL_LIST(list)->first;
}

sl_value
sl_second(struct sl_interpreter_state *state, sl_value list)
{
        return sl_first(state, sl_rest(state, list));
}

sl_value
sl_third(struct sl_interpreter_state *state, sl_value list)
{
        return sl_first(state, sl_rest(state, sl_rest(state, list)));
}

sl_value
sl_rest(struct sl_interpreter_state *state, sl_value list)
{
        assert(sl_type(list) == state->tList);
        return SL_LIST(list)->rest;
}

sl_value
sl_size(struct sl_interpreter_state *state, sl_value val)
{
        sl_value type = sl_type(val);
        assert(type == state->tList || type == state->tString);

        if (type == state->tList)
                return sl_integer_new(state, SL_LIST(val)->size);
        else
                return sl_integer_new(state, SL_STRING(val)->size);
}

sl_value
sl_reverse(struct sl_interpreter_state *state, sl_value list)
{
        assert(sl_type(list) == state->tList);

        sl_value new_list = state->sl_empty_list;

        while (sl_empty(state, list) != state->sl_true) {
                new_list = sl_list_new(state, sl_first(state, list), new_list);
                list = sl_rest(state, list);
        }

        return new_list;
}

sl_value
sl_empty(struct sl_interpreter_state *state, sl_value list)
{
        if (list == state->sl_empty_list)
                return state->sl_true;
        else
                return state->sl_false;
}

/* association lists */
/* TODO: define these in lisp eventually */

sl_value
sl_alist_has_key(struct sl_interpreter_state *state, sl_value alist, sl_value key)
{
        if (sl_empty(state, alist) == state->sl_true) {
                return state->sl_false;
        } else if (sl_equals(state, sl_first(state, sl_first(state, alist)), key) == state->sl_true) {
                return state->sl_true;
        } else {
                return sl_alist_has_key(state, sl_rest(state, alist), key);
        }
}

sl_value
sl_alist_get(struct sl_interpreter_state *state, sl_value alist, sl_value key)
{
        if (sl_empty(state, alist) == state->sl_true) {
                /* TODO: raise exception instead of dying */
                fprintf(stderr, "Error: alist does not contain %s\n", sl_string_cstring(state, sl_inspect(state, key)));
                abort();
        } else if (sl_equals(state, sl_first(state, sl_first(state, alist)), key) == state->sl_true) {
                return sl_first(state, sl_rest(state, sl_first(state, alist)));
        } else {
                return sl_alist_get(state, sl_rest(state, alist), key);
        }
}

sl_value
sl_alist_set(struct sl_interpreter_state *state, sl_value alist, sl_value key, sl_value value)
{
        if (sl_alist_has_key(state, alist, key) == state->sl_true) {
                fprintf(stderr, "Error: alist already contains key `%s'\n", sl_string_cstring(state, sl_inspect(state, key)));
                abort();
        }

        sl_value pair = sl_list_new(state, key, sl_list_new(state, value, state->sl_empty_list));

        return sl_list_new(state, pair, alist);
}


/* Hack for setting up List type before putting it in the
 * environment */
sl_value sl_type_new_without_def(struct sl_interpreter_state *state, sl_value name);

void
sl_init_list(struct sl_interpreter_state *state)
{
        state->tList = boot_type_new(state, sl_string_new(state, "List"));
        state->sl_empty_list = sl_empty_list_new(state);
}
