#include "shoelaces.h"

sl_value sl_tString;

sl_value sl_new_string(char *value)
{
        sl_value s = sl_alloc(struct SLString);
        SL_BASIC(s)->type = sl_tString;

        /* TODO: check for ENOMEM */
        SL_STRING(s)->value = strdup(value);
        SL_STRING(s)->size = sl_new_integer(strlen(SL_STRING(s)->value));

        return s;
}

char *sl_string_cstring(sl_value string)
{
        return SL_STRING(string)->value;
}

void sl_string_p(sl_value string)
{
        printf("\"%s\"\n", sl_string_cstring(string));
}

void sl_init_string()
{
        sl_tString = sl_new_type("String");
}
