#include <assert.h>

#include "shoelaces.h"
#include "internal.h"

void *
memzero(void *p, size_t length)
{
        return memset(p, 0, length);
}

sl_value
sl_c_map(struct sl_interpreter_state *state, sl_c_map_func f, sl_value list)
{
        assert(sl_is_a_list(state, list));

        if (list == state->sl_empty_list) {
                return state->sl_empty_list;
        }

        sl_value new_val = f(state, sl_first(state, list));
        sl_value rest = sl_rest(state, list);
        return sl_list_new(state, new_val, sl_c_map(state, f, rest));
}
