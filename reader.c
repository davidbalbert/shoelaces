#include "shoelaces.h"

sl_value sl_read_integer(char *input)
{
        return sl_new_integer(strtol(input, NULL, 10));
}

sl_value sl_read_string(struct sl_interpreter_state *state, char *input)
{
        if (input[0] >= '0' && input[0] <= '9') {
                return sl_read_integer(input);
        } else {
                return sl_intern(state, input);
        }
}
