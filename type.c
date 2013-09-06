#include "shoelaces.h"
#include "internal.h"

static sl_value
type_new(struct sl_interpreter_state *state, sl_value name, unsigned int abstract)
{
        sl_value t = sl_gc_alloc(state, sizeof(struct SLType));
        SL_BASIC(t)->type = state->tType;
        SL_TYPE(t)->name = name;
        SL_TYPE(t)->super = state->tAny;
        SL_TYPE(t)->abstract = abstract;

        return t;
}

sl_value
sl_type_new(struct sl_interpreter_state *state, sl_value name)
{
        sl_value t = type_new(state, name, 0);

        sl_def(state, sl_intern_string(state, name), t);
        return t;
}

sl_value
sl_abstract_type_new(struct sl_interpreter_state *state, sl_value name)
{
        sl_value t = type_new(state, name, 1);

        sl_def(state, sl_intern_string(state, name), t);
        return t;
}

/* for bootstrapping the type system */
sl_value
boot_type_new(struct sl_interpreter_state *state, sl_value name)
{
        sl_value t = type_new(state, name, 0);
        return t;
}

sl_value
boot_abstract_type_new(struct sl_interpreter_state *state, sl_value name)
{
        sl_value t = type_new(state, name, 1);
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
                return sl_list_new(state, sl_type(first), sl_types(state, rest));
        }
}

sl_value
sl_type_inspect(struct sl_interpreter_state *state, sl_value type)
{
        assert(sl_type(type) == state->tType);
        return SL_TYPE(type)->name;
}

sl_value
sl_super(struct sl_interpreter_state *state, sl_value type)
{
        if (type == state->tAny) {
                fprintf(stderr, "Error: Any does not have a supertype\n");
                abort();
        }

        return SL_TYPE(type)->super;
}

sl_value
sl_abstract(struct sl_interpreter_state *state, sl_value type)
{
        return SL_TYPE(type)->abstract ? state->sl_true : state->sl_false;
}

void
fix_type_names(struct sl_interpreter_state *state)
{
        SL_BASIC(SL_TYPE(state->tType)->name)->type = state->tString;
        SL_BASIC(SL_TYPE(state->tAny)->name)->type = state->tString;
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

        state->tAny = boot_abstract_type_new(state, sl_string_new(state, "Any"));

        SL_TYPE(state->tAny)->super = NULL;
        SL_TYPE(state->tType)->super = state->tAny;
}

void
sl_init_type(struct sl_interpreter_state *state)
{
        sl_define_function(state, "super", sl_super, 1, sl_list(state, 1, state->tType));
        sl_define_function(state, "abstract?", sl_abstract, 1, sl_list(state, 1, state->tType));
}
