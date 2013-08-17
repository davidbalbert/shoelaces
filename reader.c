#include "shoelaces.h"

sl_value sl_read_integer(char *input)
{
        return sl_new_integer(strtol(input, NULL, 10));
}

sl_value sl_parse_token(struct sl_interpreter_state *state, char *input)
{
        if (strcmp(input, "true") == 0) {
                return sl_true;
        } else if (strcmp(input, "false") == 0) {
                return sl_false;
        } else if (strcmp(input, "nil") == 0) {
                return sl_nil;
        } else if (strcmp(input, "()") == 0) {
                return sl_nil;
        } else {
                return sl_intern(state, input);
        }
}

sl_value sl_read_string(struct sl_interpreter_state *state, char *input)
{
        if (input[0] >= '0' && input[0] <= '9') {
                return sl_read_integer(input);
        } else {
                return sl_parse_token(state, input);
        }
}
