#include "shoelaces.h"

sl_value sl_tString;

sl_value sl_string_new(char *value)
{
        sl_value s = sl_alloc(struct SLString);
        SL_BASIC(s)->type = sl_tString;

        /* TODO: check for ENOMEM */
        SL_STRING(s)->value = strdup(value);
        SL_STRING(s)->size = strlen(SL_STRING(s)->value);

        return s;
}

sl_value sl_string_concat(sl_value s1, sl_value s2)
{
        char *str;
        sl_value s;
        int size;

        if (sl_type(s1) != sl_tString) {
                s1 = sl_inspect(s1);
        }

        if (sl_type(s2) != sl_tString) {
                s2 = sl_inspect(s2);
        }

        size = NUM2INT(sl_size(s1)) + NUM2INT(sl_size(s2)) + 1;
        str = sl_native_malloc(size * sizeof(char));

        sprintf(str, "%s%s", sl_string_cstring(s1), sl_string_cstring(s2));

        s = sl_string_new(str);
        free(str);

        return s;
}

char *sl_string_cstring(sl_value string)
{
        assert(sl_type(string) == sl_tString);
        return SL_STRING(string)->value;
}

sl_value sl_string_inspect(sl_value string)
{
        assert(sl_type(string) == sl_tString);
        char *str = sl_native_malloc((NUM2INT(sl_size(string)) + 1) * sizeof(char));
        sl_value s;

        sprintf(str, "\"%s\"", sl_string_cstring(string));
        s = sl_string_new(str);
        free(str);

        return s;
}

void sl_init_string()
{
        sl_tString = sl_type_new(sl_string_new("String"));
}
