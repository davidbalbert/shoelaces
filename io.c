#include <stdio.h>

#include "shoelaces.h"
#include "internal.h"

static sl_value
generic_inspect(struct sl_interpreter_state *state, sl_value val)
{
        sl_value type = sl_type(val);
        sl_value str;
        char *inspect_string = sl_native_malloc((4 + NUM2INT(sl_string_size(state, SL_TYPE(type)->name))) * sizeof(char));

        sprintf(inspect_string, "#<%s>", sl_string_cstring(state, SL_TYPE(type)->name));

        str = sl_string_new(state, inspect_string);
        free(inspect_string);

        return str;
}

sl_value
sl_inspect(struct sl_interpreter_state *state, sl_value val)
{
        sl_value f = sl_env_get(state, state->global_env, sl_intern(state, "inspect"));
        return sl_apply(state, f, sl_list(state, 1, val));
}

sl_value
p(struct sl_interpreter_state *state, sl_value val)
{
        printf("%s\n", sl_string_cstring(state, sl_inspect(state, val)));
        return val;
}

void
sl_p(struct sl_interpreter_state *state, sl_value val)
{
        sl_value f = sl_env_get(state, state->global_env, sl_intern(state, "p"));
        sl_apply(state, f, sl_list(state, 1, val));
}

void
sl_init_io(struct sl_interpreter_state *state)
{
        sl_define_function(state, "p", p, "(v:Any)");
        sl_define_function(state, "inspect", generic_inspect, "(v:Any)");
}
