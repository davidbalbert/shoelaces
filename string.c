#include "shoelaces.h"
#include "internal.h"

sl_value
sl_string_new(struct sl_interpreter_state *state, char *value)
{
        sl_value s = sl_gc_alloc(state, sizeof(struct SLString));
        SL_BASIC(s)->type = state->tString;

        /* TODO: check for ENOMEM */
        SL_STRING(s)->value = strdup(value);
        SL_STRING(s)->size = strlen(SL_STRING(s)->value);

        return s;
}

sl_value
sl_string_concat(struct sl_interpreter_state *state, sl_value s1, sl_value s2)
{
        char *str;
        sl_value s;
        int size;

        if (sl_type(s1) != state->tString) {
                s1 = sl_inspect(state, s1);
        }

        if (sl_type(s2) != state->tString) {
                s2 = sl_inspect(state, s2);
        }

        size = NUM2INT(sl_string_size(state, s1)) + NUM2INT(sl_string_size(state, s2)) + 1;
        str = sl_native_malloc(size * sizeof(char));

        sprintf(str, "%s%s", sl_string_cstring(state, s1), sl_string_cstring(state, s2));

        s = sl_string_new(state, str);
        free(str);

        return s;
}

char *
sl_string_cstring(struct sl_interpreter_state *state, sl_value string)
{
        assert(sl_type(string) == state->tString);
        return SL_STRING(string)->value;
}

sl_value
sl_string_inspect(struct sl_interpreter_state *state, sl_value string)
{
        assert(sl_type(string) == state->tString);
        char *str = sl_native_malloc((NUM2INT(sl_string_size(state, string)) + 1) * sizeof(char));
        sl_value s;

        sprintf(str, "\"%s\"", sl_string_cstring(state, string));
        s = sl_string_new(state, str);
        free(str);

        return s;
}

sl_value
sl_string_size(struct sl_interpreter_state *state, sl_value string)
{
        assert(sl_type(string) == state->tString);
        return sl_integer_new(state, SL_STRING(string)->size);
}


/* NOTE: This function is needed so that all required types can be set up
 * before we start adding things to the environment. If in doubt, you should
 * put your String initialization code into sl_init_string. */

/* NOTE: Don't call sl_define_function in boot_string. Sl_define_function
 * expects the environment to be set up. */
void
boot_string(struct sl_interpreter_state *state)
{
        state->tString = boot_type_new(state, sl_string_new(state, "String"));
}

void
sl_init_string(struct sl_interpreter_state *state)
{
        sl_define_function(state, "size", sl_string_size, 1, sl_list(state, 1, state->tString));
}
