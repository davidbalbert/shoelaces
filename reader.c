#include "shoelaces.h"

sl_value sl_read_integer(char *input)
{
        return sl_new_integer(strtol(input, NULL, 10));
}

sl_value sl_read_string(char *input)
{
        return sl_read_integer(input);
}
