#include "shoelaces.h"

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
        sl_value type = sl_type(val);
        sl_value str;

        if (state->tType == type) {
                str = sl_type_inspect(state, val);
        } else if (state->tInteger == type) {
                str = sl_integer_inspect(state, val);
        } else if (state->tSymbol == type) {
                str = sl_symbol_inspect(state, val);
        } else if (state->tBoolean == type) {
                str = sl_boolean_inspect(state, val);
        } else if (state->tList == type) {
                str = sl_list_inspect(state, val);
        } else if (state->tString == type) {
                str = sl_string_inspect(state, val);
        } else if (state->tFunction == type) {
                str = sl_function_inspect(state, val);
        } else if (state->tMethodTable == type) {
                str = sl_method_table_inspect(state, val);
        } else if (state->tMethod == type) {
                str = sl_method_inspect(state, val);
        } else {
                str = generic_inspect(state, val);
        }

        return str;
}

void
sl_p(struct sl_interpreter_state *state, sl_value val)
{
        printf("%s\n", sl_string_cstring(state, sl_inspect(state, val)));
}

void
sl_init_io(struct sl_interpreter_state *state)
{
        /*
        sl_define_function(state, "p", sl_p, 1, sl_list(state, 1, state->iAny);
        */
}
