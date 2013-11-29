#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

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

static sl_value
list_inspect(struct sl_interpreter_state *state, sl_value list)
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
                sl_value v = sl_first(state, strings);

                if (sl_type(v) == state->tString) {
                        output = sl_string_concat(state, output, v);
                } else {
                        output = sl_string_concat(state, output, sl_inspect(state, v));
                }

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
                new_size = NUM2INT(sl_list_size(state, rest)) + 1;
        } else {
                /* TODO: The size of a list should never be -1. We
                 * should raise an exception here instead because
                 * you're not allowed to call size on a dotted pair
                 * */
                new_size = -1;
        }

        return sl_alloc_list(state, first, rest, new_size);
}

sl_value sl_list(struct sl_interpreter_state *state, size_t size, ...)
{
        va_list values;
        va_start(values, size);

        sl_value list = state->sl_empty_list;

        for (int i = 0; i < size; i++) {
                list = sl_list_new(state, va_arg(values, sl_value), list);
        }

        va_end(values);

        return sl_reverse(state, list);
}

sl_value
sl_first(struct sl_interpreter_state *state, sl_value list)
{
        assert(sl_type(list) == state->tList);

        if (list == state->sl_empty_list) {
                fprintf(stderr, "Error: You can't call `first' on the empty list\n");
                abort();
        }

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
sl_fourth(struct sl_interpreter_state *state, sl_value list)
{
        return sl_first(state, sl_rest(state, sl_rest(state, sl_rest(state, list))));
}

sl_value
sl_fifth(struct sl_interpreter_state *state, sl_value list)
{
        return sl_first(state, sl_rest(state, sl_rest(state, sl_rest(state, sl_rest(state, list)))));
}

sl_value
sl_rest(struct sl_interpreter_state *state, sl_value list)
{
        assert(sl_type(list) == state->tList);

        if (list == state->sl_empty_list) {
                fprintf(stderr, "Error: You can't call `rest' on the empty list\n");
                abort();
        }

        return SL_LIST(list)->rest;
}

sl_value
sl_list_size(struct sl_interpreter_state *state, sl_value val)
{
        assert(sl_type(val) == state->tList);

        return sl_integer_new(state, SL_LIST(val)->size);
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
        } else if (sl_equal(state, sl_first(state, sl_first(state, alist)), key) == state->sl_true) {
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
        } else if (sl_equal(state, sl_first(state, sl_first(state, alist)), key) == state->sl_true) {
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

sl_value
sl_alist_values(struct sl_interpreter_state *state, sl_value alist)
{
        if (alist == state->sl_empty_list) {
                return state->sl_empty_list;
        } else {
                return sl_list_new(state, sl_second(state, sl_first(state, alist)), sl_alist_values(state, sl_rest(state, alist)));
        }
}

sl_value
sl_list_contains(struct sl_interpreter_state *state, sl_value list, sl_value item)
{
        if (list == state->sl_empty_list) {
                return state->sl_false;
        } else if (sl_equal(state, item, sl_first(state, list)) == state->sl_true) {
                return state->sl_true;
        } else {
                return sl_list_contains(state, sl_rest(state, list), item);
        }
}

static sl_value
concat_2(struct sl_interpreter_state *state, sl_value l1, sl_value l2)
{
        if (l1 == state->sl_empty_list) {
                return l2;
        } else {
                return sl_list_new(state, sl_first(state, l1), concat_2(state, sl_rest(state, l1), l2));
        }
}

sl_value
sl_list_concat(struct sl_interpreter_state *state, sl_value lists)
{
        sl_value ret = state->sl_empty_list;

        lists = sl_reverse(state, lists);

        for (sl_value l = lists; l != state->sl_empty_list; l = sl_rest(state, l)) {
                ret = concat_2(state, sl_first(state, l), ret);
        }

        return ret;
}

/* NOTE: This function is needed so that all required types can be set up
 * before we start adding things to the environment. If in doubt, you should
 * put your List initialization code into sl_init_list. */

/* NOTE: Don't call sl_define_function in boot_list. Sl_define_function expects
 * the environment to be set up. */
void
boot_list(struct sl_interpreter_state *state)
{
        state->sl_empty_list = sl_empty_list_new(state);
        state->tList = boot_abstract_type_new(state, sl_string_new(state, "List"), NULL);

        /* TODO: Make this actually be List{None} */
        SL_BASIC(state->sl_empty_list)->type = state->tList;
}

void
sl_init_list(struct sl_interpreter_state *state)
{
        SL_TYPE(state->tList)->parameters = sl_list(state, 1, sl_intern(state, "T"));

        sl_define_function(state, "first", sl_first, sl_list(state, 1, state->tList));
        sl_define_function(state, "second", sl_second, sl_list(state, 1, state->tList));
        sl_define_function(state, "third", sl_third, sl_list(state, 1, state->tList));
        sl_define_function(state, "fourth", sl_fourth, sl_list(state, 1, state->tList));
        sl_define_function(state, "fifth", sl_fifth, sl_list(state, 1, state->tList));
        sl_define_function(state, "rest", sl_rest, sl_list(state, 1, state->tList));
        sl_define_function(state, "size", sl_list_size, sl_list(state, 1, state->tList));
        sl_define_function(state, "reverse", sl_reverse, sl_list(state, 1, state->tList));
        sl_define_function(state, "empty?", sl_empty, sl_list(state, 1, state->tList));
        sl_define_function(state, "join", sl_list_join, sl_list(state, 2, state->tList, state->tString));
        sl_define_function(state, "contains?", sl_list_contains, sl_list(state, 2, state->tList, state->tAny));
        sl_define_function(state, "inspect", list_inspect, sl_list(state, 1, state->tList));
        sl_define_function(state, "concat", sl_list_concat, sl_list(state, 2, state->s_ampersand, state->tList));
}
