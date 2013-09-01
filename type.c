#include "shoelaces.h"
#include "internal.h"

sl_value
sl_type_new(struct sl_interpreter_state *state, sl_value name)
{
        sl_value t = sl_gc_alloc(state, sizeof(struct SLType));
        SL_BASIC(t)->type = state->tType;
        SL_TYPE(t)->name = name;

        sl_def(state, sl_intern_string(state, name), t);
        return t;
}

/* for bootstrapping the type system */
sl_value
boot_type_new(struct sl_interpreter_state *state, sl_value name)
{
        sl_value t = sl_gc_alloc(state, sizeof(struct SLType));
        SL_BASIC(t)->type = state->tType;
        SL_TYPE(t)->name = name;
        return t;
}

sl_value
boot_def_type(struct sl_interpreter_state *state, sl_value type)
{
        assert(sl_type(type) == state->tType);

        sl_value sym = sl_intern_string(state, SL_TYPE(type)->name);
        return sl_def(state, sym, type);
}

sl_value
sl_type(sl_value object)
{
        return SL_BASIC(object)->type;
}

sl_value
sl_types(struct sl_interpreter_state *state, sl_value values)
{
        assert(sl_type(values) == state->tList);

        if (values == state->sl_empty_list) {
                return state->sl_empty_list;
        } else {
                sl_value first = sl_first(state, values);
                sl_value rest = sl_rest(state, values);
                return sl_list_new(state, sl_type(first), rest);
        }
}

sl_value
sl_type_inspect(struct sl_interpreter_state *state, sl_value type)
{
        assert(sl_type(type) == state->tType);

        sl_value name = sl_string_concat(state, sl_string_new(state, "#<Type: "), SL_TYPE(type)->name);
        name = sl_string_concat(state, name, sl_string_new(state, ">"));
        return name;
}


void
fix_type_names(struct sl_interpreter_state *state)
{
        SL_BASIC(SL_TYPE(state->tType)->name)->type = state->tString;
        SL_BASIC(SL_TYPE(state->tString)->name)->type = state->tString;
}


/* NOTE: This function is needed so that all required types can be set up
 * before we start adding things to the environment. If in doubt, you should
 * put your Type initialization code into sl_init_type. */

/* NOTE: Don't call sl_define_function in boot_type. Sl_define_function expects
 * the environment to be set up. */
void
boot_type(struct sl_interpreter_state *state)
{
        state->tType = boot_type_new(state, sl_string_new(state, "Type"));
        SL_BASIC(state->tType)->type = state->tType;
}

void
sl_init_type(struct sl_interpreter_state *state)
{
}
