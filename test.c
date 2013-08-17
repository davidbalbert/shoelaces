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

void test_read_integer()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value a = sl_read_string(state, "1");
        sl_value b = sl_new_integer(1);

        assert(sl_type(a) == sl_tInteger);
        assert(NUM2INT(a) == NUM2INT(b));

        sl_destroy(state);
}

void test_read_sym()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value a = sl_read_string(state, "foo");
        sl_value b = sl_intern(state, "foo");

        assert(a == b);
        assert(sl_type(a) == sl_tSymbol);

        sl_destroy(state);
}

void test_read_boolean()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value t = sl_read_string(state, "true");

        assert(sl_type(t) == sl_tBoolean);
        assert(t == sl_true);

        sl_value f = sl_read_string(state, "false");

        assert(sl_type(f) == sl_tBoolean);
        assert(f == sl_false);

        sl_destroy(state);
}

void test_read_nil_and_empty_list()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value nil = sl_read_string(state, "nil");

        assert(sl_type(nil) == sl_tList);
        assert(sl_nil == nil);
        assert(NUM2INT(sl_size(nil)) == 0);

        sl_value empty_list = sl_read_string(state, "()");

        assert(sl_type(empty_list) == sl_tList);
        assert(sl_nil == empty_list);

        sl_destroy(state);
}


int main(int argc, char *argv[])
{
        test_symbol_table();
        test_read_integer();
        test_read_sym();
        test_read_boolean();
        test_read_nil_and_empty_list();

        printf("Tests passed!\n");
        return 0;
}
