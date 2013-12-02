#include <assert.h>

#include "shoelaces.h"
#include "internal.h"

static
sl_value new_keyword(struct sl_interpreter_state *state, sl_value name)
{
        sl_value sym = sl_gc_alloc(state, sizeof(struct SLKeyword));
        SL_BASIC(sym)->type = state->tKeyword;
        SL_SYMBOL(sym)->name = name;
        return sym;
}

static sl_value
keyword_table_get(struct sl_interpreter_state *state, char *name)
{
        khiter_t iter;

        iter = kh_get(str, state->keyword_table, name);

        if (iter == kh_end(state->keyword_table)) {
                return SLUndefined;
        } else {
                return kh_value(state->keyword_table, iter);
        }
}

static void
keyword_table_put(struct sl_interpreter_state *state, char *name, sl_value value)
{
        assert(sl_type(value) == state->tKeyword);

        /* TODO: Check for ENOMEM */
        char *duped_name = strdup(name);
        int ret;
        khiter_t iter;
        iter = kh_put(str, state->keyword_table, duped_name, &ret);
        kh_value(state->keyword_table, iter) = value;
}

size_t
sl_keyword_table_size(struct sl_interpreter_state *state)
{
        return kh_size(state->keyword_table);
}

/* NOTE: name is expected to _not_ have the preceding ':' */
sl_value
sl_intern_keyword(struct sl_interpreter_state *state, char *name)
{
        sl_value keyword;

        if ((keyword = keyword_table_get(state, name))) {
                return keyword;
        } else {
                keyword = new_keyword(state, sl_string_new(state, name));
                keyword_table_put(state, name, keyword);
                return keyword;
        }
}

static sl_value
keyword_inspect(struct sl_interpreter_state *state, sl_value keyword)
{
        assert(sl_type(keyword) == state->tKeyword);
        return sl_string_concat(state, sl_string_new(state, ":"), SL_KEYWORD(keyword)->name);
}

/* NOTE: This function is needed so that all required types can be set up
 * before we start adding things to the environment. If in doubt, you should
 * put your Keyword initialization code into sl_init_keyword. */

/* NOTE: Don't call sl_define_function in boot_keyword. Sl_define_function
 * expects the environment to be set up. */
void
boot_keyword(struct sl_interpreter_state *state)
{
        state->tKeyword = boot_type_new(state, sl_string_new(state, "Keyword"));
}

void
sl_init_keyword(struct sl_interpreter_state *state)
{
        sl_define_function(state, "inspect", keyword_inspect, "(kw:Keyword)");
}
