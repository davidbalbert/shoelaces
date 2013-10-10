#include "shoelaces.h"

void
test_read_integer()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value a = sl_read(state, "1");
        sl_value b = sl_integer_new(state, 1);

        assert(sl_type(a) == state->tInteger);
        assert(NUM2INT(a) == NUM2INT(b));

        sl_destroy(state);
}

void
test_read_sym()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value a = sl_read(state, "foo");
        sl_value b = sl_intern(state, "foo");

        assert(a == b);
        assert(sl_type(a) == state->tSymbol);

        sl_destroy(state);
}

void
test_read_boolean()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value t = sl_read(state, "true");

        assert(sl_type(t) == state->tBoolean);
        assert(t == state->sl_true);

        sl_value f = sl_read(state, "false");

        assert(sl_type(f) == state->tBoolean);
        assert(f == state->sl_false);

        sl_destroy(state);
}

void
test_read_string()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value str = sl_read(state, "\"Hello, world!\"");

        assert(sl_type(str) == state->tString);
        assert(NUM2INT(sl_string_size(state, str)) == strlen("Hello, world!"));
        assert(strcmp(sl_string_cstring(state, str), "Hello, world!") == 0);

        sl_destroy(state);
}

void
test_read_empty_list()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value empty_list = sl_read(state, "()");

        assert(sl_type(empty_list) == state->tList);
        assert(state->sl_empty_list == empty_list);

        sl_destroy(state);
}

void
test_read_list()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value list = sl_read(state, "(123 true sym \"string\")");
        sl_value v;

        assert(sl_type(list) == state->tList);
        assert(NUM2INT(sl_list_size(state, list)) == 4);

        assert(sl_type(sl_first(state, list)) == state->tInteger);
        assert(NUM2INT(sl_first(state, list)) == 123);

        assert(sl_first(state, sl_rest(state, list)) == state->sl_true);

        assert(sl_first(state, sl_rest(state, sl_rest(state, list))) == sl_intern(state, "sym"));

        v = sl_first(state, sl_rest(state, sl_rest(state, sl_rest(state, list))));
        assert(sl_type(v) == state->tString);
        assert(strcmp(sl_string_cstring(state, v), "string") == 0);

        sl_destroy(state);
}

void
test_read_nested_list()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value list = sl_read(state, "(a (b c) d)");

        assert(sl_type(list) == state->tList);
        assert(NUM2INT(sl_list_size(state, list)) == 3);

        assert(sl_first(state, list) == sl_intern(state, "a"));

        sl_value nested_list = sl_first(state, sl_rest(state, list));
        assert(sl_type(nested_list) == state->tList);
        assert(NUM2INT(sl_list_size(state, nested_list)) == 2);
        assert(sl_first(state, nested_list) == sl_intern(state, "b"));
        assert(sl_first(state, sl_rest(state, nested_list)) == sl_intern(state, "c"));

        assert(sl_first(state, sl_rest(state, sl_rest(state, list))) == sl_intern(state, "d"));

        sl_destroy(state);
}

void
test_read_quote()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value list = sl_read(state, "'a");

        assert(sl_type(list) == state->tList);
        assert(sl_first(state, list) == sl_intern(state, "quote"));

        sl_value a = sl_first(state, sl_rest(state, list));
        assert(a == sl_intern(state, "a"));

        sl_destroy(state);
}

void
test_read_dotted_pair()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value dotted_pair = sl_read(state, "(1 . 2)");

        assert(sl_type(dotted_pair) == state->tList);

        assert(sl_type(sl_first(state, dotted_pair)) == state->tInteger);
        assert(NUM2INT(sl_first(state, dotted_pair)) == 1);

        assert(sl_type(sl_rest(state, dotted_pair)) == state->tInteger);
        assert(NUM2INT(sl_rest(state, dotted_pair)) == 2);

        sl_destroy(state);
}

void
test_read_keyword()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value kw = sl_read(state, ":foo");

        assert(sl_type(kw) == state->tKeyword);
        assert(kw == sl_intern_keyword(state, "foo"));

        sl_destroy(state);
}

void
test_read_colon()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value colon = sl_read(state, ":");

        assert(colon == sl_intern(state, ":"));

        sl_destroy(state);
}

void
test_read_type_expression()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value actual = sl_read(state, "Foo{Bar}");
        sl_value expected = sl_list(state, 3, sl_intern(state, "apply-type"), sl_intern(state, "Foo"), sl_intern(state, "Bar"));

        assert(sl_equals(state, expected, actual) == state->sl_true);

        sl_destroy(state);
}

void
test_gc()
{
        struct sl_interpreter_state *state = sl_init();
        size_t old_object_count;

        /* run gc to get rid of any intermediate objects created during boot */
        sl_gc_run(state);

        old_object_count = sl_gc_heap_size(state);

        for (int i = 0; i < 12345; i++) {
                sl_string_new(state, "hello, world");
        }

        sl_gc_run(state);

        assert(sl_gc_heap_size(state) == old_object_count);

        sl_destroy(state);
}

void
test_eval_type()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "Type");
        sl_value out = sl_eval(state, in, state->global_env);

        assert(out == state->tType);

        sl_destroy(state);
}

void
test_eval_number()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "100");
        sl_value out = sl_eval(state, in, state->global_env);

        assert(NUM2INT(out) == 100);

        sl_destroy(state);
}

void
test_eval_string()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "\"hello, world\"");
        sl_value out = sl_eval(state, in, state->global_env);

        assert(sl_equals(state, out, sl_string_new(state, "hello, world")) == state->sl_true);

        sl_destroy(state);
}

void
test_eval_boolean()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "true");
        sl_value out = sl_eval(state, in, state->global_env);

        assert(out == state->sl_true);

        in = sl_read(state, "false");
        out = sl_eval(state, in, state->global_env);

        assert(out == state->sl_false);

        sl_destroy(state);
}

void
test_eval_def()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "(def foo 42)");
        sl_value out = sl_eval(state, in, state->global_env);

        assert(NUM2INT(out) == 42);

        in = sl_read(state, "foo");
        out = sl_eval(state, in, state->global_env);

        assert(NUM2INT(out) == 42);

        sl_destroy(state);
}

void
test_eval_quote()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "'foo");
        sl_value out = sl_eval(state, in, state->global_env);

        assert(sl_intern(state, "foo") == out);

        sl_destroy(state);
}

void
test_eval_if()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "(if true 'yay 'boo)");
        sl_value out = sl_eval(state, in, state->global_env);

        assert(sl_intern(state, "yay") == out);

        in = sl_read(state, "(if 1000 'yay 'boo)");
        out = sl_eval(state, in, state->global_env);

        assert(sl_intern(state, "yay") == out);

        in = sl_read(state, "(if false 'yay 'boo)");
        out = sl_eval(state, in, state->global_env);

        assert(sl_intern(state, "boo") == out);

        sl_destroy(state);
}

void
test_eval_empty_list()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "()");
        sl_value out = sl_eval(state, in, state->global_env);

        assert(out == state->sl_empty_list);

        sl_destroy(state);
}

void
test_eval_eval()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "(eval ''foo)");
        sl_value out = sl_eval(state, in, state->global_env);

        assert(sl_intern(state, "foo") == out);

        sl_destroy(state);
}

void
test_eval_function_call()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "(first '(a b c))");
        sl_value out = sl_eval(state, in, state->global_env);

        assert(sl_intern(state, "a") == out);

        sl_destroy(state);
}

void
test_eval_symbol()
{
        struct sl_interpreter_state *state = sl_init();

        sl_value in = sl_read(state, "'foo");

        sl_value out1 = sl_eval(state, in, state->global_env);
        sl_value out2 = sl_eval(state, in, state->global_env);

        assert(out1 == out2);

        sl_destroy(state);
}

int
main(int argc, char *argv[])
{
        /* reader */
        test_read_integer();
        test_read_sym();
        test_read_boolean();
        test_read_string();
        test_read_list();
        test_read_nested_list();
        test_read_empty_list();
        test_read_quote();
        test_read_dotted_pair();
        test_read_keyword();
        test_read_colon();
        test_read_type_expression();

        test_gc();

        test_eval_type();
        test_eval_number();
        test_eval_string();
        test_eval_boolean();
        test_eval_def();
        test_eval_quote();
        test_eval_if();
        test_eval_empty_list();
        test_eval_symbol();

        test_eval_function_call();
        /*test_eval_eval();*/

        printf("Tests passed!\n");
        return 0;
}
