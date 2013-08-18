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

        sl_value a = sl_read(state, "1");
        sl_value b = sl_new_integer(1);

        assert(sl_type(a) == sl_tInteger);
        assert(NUM2INT(a) == NUM2INT(b));

        sl_destroy(state);
}

void test_read_sym()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value a = sl_read(state, "foo");
        sl_value b = sl_intern(state, "foo");

        assert(a == b);
        assert(sl_type(a) == sl_tSymbol);

        sl_destroy(state);
}

void test_read_boolean()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value t = sl_read(state, "true");

        assert(sl_type(t) == sl_tBoolean);
        assert(t == sl_true);

        sl_value f = sl_read(state, "false");

        assert(sl_type(f) == sl_tBoolean);
        assert(f == sl_false);

        sl_destroy(state);
}

void test_read_string()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value str = sl_read(state, "\"Hello, world!\"");

        assert(sl_type(str) == sl_tString);
        assert(NUM2INT(sl_size(str)) == strlen("Hello, world!"));
        assert(strcmp(sl_string_cstring(str), "Hello, world!") == 0);

        sl_destroy(state);
}

void test_read_empty_list()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value empty_list = sl_read(state, "()");

        assert(sl_type(empty_list) == sl_tList);
        assert(sl_empty_list == empty_list);

        sl_destroy(state);
}

void test_read_list()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value list = sl_read(state, "(123 true sym \"string\")");
        sl_value v;

        assert(sl_type(list) == sl_tList);
        assert(NUM2INT(sl_size(list)) == 4);

        assert(sl_type(sl_first(list)) == sl_tInteger);
        assert(NUM2INT(sl_first(list)) == 123);

        assert(sl_first(sl_rest(list)) == sl_true);

        assert(sl_first(sl_rest(sl_rest(list))) == sl_intern(state, "sym"));

        v = sl_first(sl_rest(sl_rest(sl_rest(list))));
        assert(sl_type(v) == sl_tString);
        assert(strcmp(sl_string_cstring(v), "string") == 0);

        sl_destroy(state);
}

void test_read_nested_list()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value list = sl_read(state, "(a (b c) d)");

        assert(sl_type(list) == sl_tList);
        assert(NUM2INT(sl_size(list)) == 3);

        assert(sl_first(list) == sl_intern(state, "a"));

        sl_value nested_list = sl_first(sl_rest(list));
        assert(sl_type(nested_list) == sl_tList);
        assert(NUM2INT(sl_size(nested_list)) == 2);
        assert(sl_first(nested_list) == sl_intern(state, "b"));
        assert(sl_first(sl_rest(nested_list)) == sl_intern(state, "c"));

        assert(sl_first(sl_rest(sl_rest(list))) == sl_intern(state, "d"));

        sl_destroy(state);
}

void test_read_quote()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value list = sl_read(state, "'a");

        assert(sl_type(list) == sl_tList);
        assert(sl_first(list) == sl_intern(state, "quote"));

        sl_value a = sl_first(sl_rest(list));
        assert(a == sl_intern(state, "a"));

        sl_destroy(state);
}

void test_read_dotted_pair()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value dotted_pair = sl_read(state, "(1 . 2)");

        assert(sl_type(dotted_pair) == sl_tList);

        assert(sl_type(sl_first(dotted_pair)) == sl_tInteger);
        assert(NUM2INT(sl_first(dotted_pair)) == 1);

        assert(sl_type(sl_rest(dotted_pair)) == sl_tInteger);
        assert(NUM2INT(sl_rest(dotted_pair)) == 2);

        sl_destroy(state);
}


int main(int argc, char *argv[])
{
        test_symbol_table();
        test_read_integer();
        test_read_sym();
        test_read_boolean();
        test_read_string();
        test_read_list();
        test_read_nested_list();
        test_read_empty_list();
        test_read_quote();
        test_read_dotted_pair();

        printf("Tests passed!\n");
        return 0;
}
