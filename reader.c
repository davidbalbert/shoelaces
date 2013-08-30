#include "shoelaces.h"

struct sl_reader
{
        char *input;
        char *start_position;
        char *end_position;
};

typedef sl_value (*reader_macro)(struct sl_interpreter_state *state, struct sl_reader *reader);
static reader_macro reader_macros[256];

static struct sl_reader *
new_reader(char *input)
{
        struct sl_reader *reader = sl_native_malloc(sizeof(struct sl_reader));
        reader->input = input;
        reader->start_position = input;
        reader->end_position = input;
        return reader;
}

static void
skip_one(struct sl_reader *reader)
{
        reader->start_position++;
        reader->end_position = reader->start_position;
}

static void
unread_one(struct sl_reader *reader)
{
        reader->end_position--;
        if (reader->start_position > reader->end_position) {
                reader->start_position = reader->end_position;
        }
}

static void
advance_one(struct sl_reader *reader)
{
        reader->end_position++;
}

static int
peek(struct sl_reader *reader)
{
        return *reader->end_position;
}

static int
at_end_of_input(struct sl_reader *reader)
{
        return *reader->end_position == '\0';
}

static int
at_end_of_list(struct sl_reader *reader)
{
        return *reader->end_position == ')';
}

static int
next_char_is_whitespace(struct sl_reader *reader)
{
        int ch = *reader->end_position;

        return isspace(ch);
}

static void
eat_whitespace(struct sl_reader *reader)
{
        while (next_char_is_whitespace(reader)) {
                skip_one(reader);
        }
}

static void
eat_through_end_of_list(struct sl_reader *reader)
{
        while (!at_end_of_list(reader) && !at_end_of_input(reader)) {
                skip_one(reader);
        }

        if (!at_end_of_input(reader)) {
                skip_one(reader);
        }
}

static int
next_char_is_digit(struct sl_reader *reader)
{
        int ch = *reader->end_position;

        return isdigit(ch);
}

static int
next_char_is_list_delimeter(struct sl_reader *reader)
{
        int ch = *reader->end_position;

        return ch == '(' || ch == ')';
}

static int
token_length(struct sl_reader *reader)
{
        return reader->end_position - reader->start_position;
}

static void
reader_error(struct sl_reader *reader, char *format, ...)
{
        char *format_with_prelude;
        char *prelude;
        va_list arglist;

        prelude = "Error: ";
        asprintf(&format_with_prelude, "%s%s", prelude, format);

        va_start(arglist, format);
        vprintf(format_with_prelude, arglist);
        va_end(arglist);

        free(format_with_prelude);

        exit(1);
}

/*
 * You must free the returned token yourself
 */
static char *
get_token(struct sl_reader *reader)
{
        int length = token_length(reader);
        char *token = sl_native_malloc(sizeof(char) * length + 1);

        strncpy(token, reader->start_position, length);
        token[length] = '\0';

        reader->start_position = reader->end_position;

        return token;
}

static char *
read_token(struct sl_reader *reader)
{
        while (1) {
                if (next_char_is_whitespace(reader) || next_char_is_list_delimeter(reader) || at_end_of_input(reader)) {
                        return get_token(reader);
                } else {
                        advance_one(reader);
                }
        }
}

static sl_value
parse_token(struct sl_interpreter_state *state, char *token)
{
        if (strcmp(token, "true") == 0) {
                return sl_true;
        } else if (strcmp(token, "false") == 0) {
                return sl_false;
        } else {
                return sl_intern(state, token);
        }
}

static sl_value
read_integer(struct sl_interpreter_state *state, struct sl_reader *reader)
{
        char *token;
        sl_value number;

        while (1) {
                if (next_char_is_digit(reader)) {
                        advance_one(reader);
                } else if (next_char_is_whitespace(reader) || next_char_is_list_delimeter(reader) || at_end_of_input(reader)) {
                        token = get_token(reader);
                        number = sl_integer_new(state, strtol(token, NULL, 10));
                        free(token);
                        return number;
                } else {
                        reader_error(reader, "Expecting digit, but got %c", peek(reader));
                }
        }
}

static sl_value
read(struct sl_interpreter_state *state, struct sl_reader *reader)
{
        unsigned char ch;

        while (1) {
                eat_whitespace(reader);

                if (at_end_of_input(reader)) {
                        return NULL;
                }

                ch = peek(reader);
                if (ch == '.') {
                        skip_one(reader);
                        if (next_char_is_whitespace(reader) || at_end_of_input(reader)) {
                                reader_error(reader, "Unexpected '.' outside dotted pair");
                        }

                        unread_one(reader);
                }


                if (next_char_is_digit(reader)) {
                        return read_integer(state, reader);
                }

                ch = peek(reader);
                if (reader_macros[ch]) {
                        skip_one(reader);
                        return reader_macros[ch](state, reader);
                }

                return parse_token(state, read_token(reader));
        }
}

static sl_value
read_list(struct sl_interpreter_state *state, struct sl_reader *reader)
{
        sl_value list = sl_empty_list;
        sl_value new_list;
        sl_value val;

        while (1) {
                eat_whitespace(reader);

                if (at_end_of_input(reader)) {
                        reader_error(reader, "Reached EOF, but expected ')'");
                }

                if ('.' == peek(reader)) {
                        skip_one(reader);

                        if (next_char_is_whitespace(reader)) {
                                val = read(state, reader);

                                eat_whitespace(reader);

                                if (at_end_of_input(reader)) {
                                        reader_error(reader, "Reached EOF, but expected ')'");
                                }

                                if (!at_end_of_list(reader)) {
                                        eat_through_end_of_list(reader);
                                        reader_error(reader, "More than one object follows '.' in list");
                                }

                                new_list = val;
                                while (sl_empty(list) != sl_true) {
                                        new_list = sl_list_new(state, sl_first(list), new_list);
                                        list = sl_rest(list);
                                }

                                return new_list;
                        }
                }

                if (at_end_of_list(reader)) {
                        skip_one(reader);
                        break;
                }

                val = read(state, reader);

                /* val will be NULL only if we're at the end of input, but we
                 * checked for that already. */
                assert(val != NULL);

                list = sl_list_new(state, val, list);
        }

        return sl_reverse(state, list);
}

static sl_value
read_string(struct sl_interpreter_state *state, struct sl_reader *reader)
{
        char *token;
        sl_value str;

        while (peek(reader) != '"' && !at_end_of_input(reader)) {
                advance_one(reader);
        }

        if (at_end_of_input(reader)) {
                reader_error(reader, "Expected close quote, but got EOF");
        }

        token = get_token(reader);

        skip_one(reader); /* the closing quote */

        str = sl_string_new(state, token);

        free(token);

        return str;
}

static sl_value
read_quote(struct sl_interpreter_state *state, struct sl_reader *reader)
{
        sl_value list = sl_list_new(state, read(state, reader), sl_empty_list);
        return sl_list_new(state, sl_intern(state, "quote"), list);
}

sl_value
sl_read(struct sl_interpreter_state *state, char *input)
{
        struct sl_reader *reader = new_reader(input);
        sl_value val = read(state, reader);
        free(reader);

        return val;
}

void
sl_init_reader(struct sl_interpreter_state *state)
{
        memzero(reader_macros, 256 * sizeof(reader_macro));

        reader_macros['('] = read_list;
        reader_macros['\''] = read_quote;
        reader_macros['"'] = read_string;
}
