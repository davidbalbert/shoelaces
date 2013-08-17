#include <assert.h>
#include <stdio.h>

#include "shoelaces.h"

void test_symbol_table()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value sym = sl_symbol_table_get(state, "foo");
        assert(sym == NULL);

        sl_symbol_table_put(state, "foo", (sl_value)1);
        sym = sl_symbol_table_get(state, "foo");
        assert(sym == (sl_value)1);

        sl_destroy(state);
}

void test_read_integer(struct sl_interpreter_state *state)
{
        sl_value a = sl_read_string(state, "1");
        sl_value b = sl_new_integer(1);

        assert(NUM2INT(a) == NUM2INT(b));
}

void test_read_sym(struct sl_interpreter_state *state)
{
        sl_value a = sl_read_string(state, "foo");
        sl_value b = sl_intern(state, "foo");

        assert(a == b);
}

int main(int argc, char *argv[])
{
        test_symbol_table();

        struct sl_interpreter_state *state = sl_init();
        test_read_integer(state);
        test_read_sym(state);

        printf("Tests passed!\n");

        sl_destroy(state);
        return 0;
}
