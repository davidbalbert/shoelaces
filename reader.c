#include "shoelaces.h"

struct sl_reader
{
        char *input;
        char *start_position;
        char *end_position;
};

typedef sl_value (*reader_macro)(struct sl_interpreter_state *state, struct sl_reader *reader);
static reader_macro reader_macros[256];

static struct sl_reader *sl_reader_new(char *input)
{
        struct sl_reader *reader = malloc(sizeof(struct sl_reader));
        reader->input = input;
        reader->start_position = input;
        reader->end_position = input;
        return reader;
}

static void sl_reader_skip_one(struct sl_reader *reader)
{
        reader->start_position++;
        reader->end_position = reader->start_position;
}

static void sl_reader_unread_one(struct sl_reader *reader)
{
        reader->end_position--;
        if (reader->start_position > reader->end_position) {
                reader->start_position = reader->end_position;
        }
}

static void sl_reader_advance_one(struct sl_reader *reader)
{
        reader->end_position++;
}

static int sl_reader_peek(struct sl_reader *reader)
{
        return *reader->end_position;
}

static int sl_reader_at_end_of_input(struct sl_reader *reader)
{
        return *reader->end_position == '\0';
}

static int sl_reader_at_end_of_list(struct sl_reader *reader)
{
        return *reader->end_position == ')';
}

static int sl_reader_next_char_is_whitespace(struct sl_reader *reader)
{
        int ch = *reader->end_position;

        return isspace(ch);
}

static void sl_reader_eat_whitespace(struct sl_reader *reader)
{
        while (sl_reader_next_char_is_whitespace(reader)) {
                sl_reader_skip_one(reader);
        }
}

static void sl_reader_eat_through_end_of_list(struct sl_reader *reader)
{
        while (!sl_reader_at_end_of_list(reader) && !sl_reader_at_end_of_input(reader)) {
                sl_reader_skip_one(reader);
        }

        if (!sl_reader_at_end_of_input(reader)) {
                sl_reader_skip_one(reader);
        }
}

static int sl_reader_next_char_is_digit(struct sl_reader *reader)
{
        int ch = *reader->end_position;

        return isdigit(ch);
}

static int sl_reader_next_char_is_list_delimeter(struct sl_reader *reader)
{
        int ch = *reader->end_position;

        return ch == '(' || ch == ')';
}

static int sl_reader_token_length(struct sl_reader *reader)
{
        return reader->end_position - reader->start_position;
}

static void sl_reader_error(struct sl_reader *reader, char *format, ...)
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
static char *sl_reader_get_token(struct sl_reader *reader)
{
        int length = sl_reader_token_length(reader);
        char *token = malloc(sizeof(char) * length + 1);

        strncpy(token, reader->start_position, length);
        token[length] = '\0';

        reader->start_position = reader->end_position;

        return token;
}

static char *sl_reader_read_token(struct sl_reader *reader)
{
        while (1) {
                if (sl_reader_next_char_is_whitespace(reader) || sl_reader_next_char_is_list_delimeter(reader) || sl_reader_at_end_of_input(reader)) {
                        return sl_reader_get_token(reader);
                } else {
                        sl_reader_advance_one(reader);
                }
        }
}

static sl_value sl_reader_parse_token(struct sl_interpreter_state *state, char *token)
{
        if (strcmp(token, "true") == 0) {
                return sl_true;
        } else if (strcmp(token, "false") == 0) {
                return sl_false;
        } else {
                return sl_intern(state, token);
        }
}

static sl_value sl_reader_read_integer(struct sl_reader *reader)
{
        char *token;
        sl_value number;

        while (1) {
                if (sl_reader_next_char_is_digit(reader)) {
                        sl_reader_advance_one(reader);
                } else if (sl_reader_next_char_is_whitespace(reader) || sl_reader_next_char_is_list_delimeter(reader) || sl_reader_at_end_of_input(reader)) {
                        token = sl_reader_get_token(reader);
                        number = sl_integer_new(strtol(token, NULL, 10));
                        free(token);
                        return number;
                } else {
                        sl_reader_error(reader, "Expecting digit, but got %c", sl_reader_peek(reader));
                }
        }
}

static sl_value sl_reader_read(struct sl_interpreter_state *state, struct sl_reader *reader)
{
        unsigned char ch;

        while (1) {
                sl_reader_eat_whitespace(reader);

                if (sl_reader_at_end_of_input(reader)) {
                        return NULL;
                }

                ch = sl_reader_peek(reader);
                if (ch == '.') {
                        sl_reader_skip_one(reader);
                        if (sl_reader_next_char_is_whitespace(reader) || sl_reader_at_end_of_input(reader)) {
                                sl_reader_error(reader, "Unexpected '.' outside dotted pair");
                        }

                        sl_reader_unread_one(reader);
                }


                if (sl_reader_next_char_is_digit(reader)) {
                        return sl_reader_read_integer(reader);
                }

                ch = sl_reader_peek(reader);
                if (reader_macros[ch]) {
                        sl_reader_skip_one(reader);
                        return reader_macros[ch](state, reader);
                }

                return sl_reader_parse_token(state, sl_reader_read_token(reader));
        }
}

static sl_value sl_reader_read_list(struct sl_interpreter_state *state, struct sl_reader *reader)
{
        sl_value list = sl_empty_list;
        sl_value new_list;
        sl_value val;

        while (1) {
                sl_reader_eat_whitespace(reader);

                if (sl_reader_at_end_of_input(reader)) {
                        sl_reader_error(reader, "Reached EOF, but expected ')'");
                }

                if ('.' == sl_reader_peek(reader)) {
                        sl_reader_skip_one(reader);

                        if (sl_reader_next_char_is_whitespace(reader)) {
                                val = sl_reader_read(state, reader);

                                sl_reader_eat_whitespace(reader);

                                if (sl_reader_at_end_of_input(reader)) {
                                        sl_reader_error(reader, "Reached EOF, but expected ')'");
                                }

                                if (!sl_reader_at_end_of_list(reader)) {
                                        sl_reader_eat_through_end_of_list(reader);
                                        sl_reader_error(reader, "More than one object follows '.' in list");
                                }

                                new_list = val;
                                while (sl_empty(list) != sl_true) {
                                        new_list = sl_list_new(sl_first(list), new_list);
                                        list = sl_rest(list);
                                }

                                return new_list;
                        }
                }

                if (sl_reader_at_end_of_list(reader)) {
                        sl_reader_skip_one(reader);
                        break;
                }

                val = sl_reader_read(state, reader);

                /* val will be NULL only if we're at the end of input, but we
                 * checked for that already. */
                assert(val != NULL);

                list = sl_list_new(val, list);
        }

        return sl_reverse(list);
}

static sl_value sl_reader_read_string(struct sl_interpreter_state *state, struct sl_reader *reader)
{
        char *token;
        sl_value str;

        while (sl_reader_peek(reader) != '"' && !sl_reader_at_end_of_input(reader)) {
                sl_reader_advance_one(reader);
        }

        if (sl_reader_at_end_of_input(reader)) {
                sl_reader_error(reader, "Expected close quote, but got EOF");
        }

        token = sl_reader_get_token(reader);

        sl_reader_skip_one(reader); /* the closing quote */

        str = sl_string_new(token);

        free(token);

        return str;
}

static sl_value sl_reader_read_quote(struct sl_interpreter_state *state, struct sl_reader *reader)
{
        sl_value list = sl_list_new(sl_reader_read(state, reader), sl_empty_list);
        return sl_list_new(sl_intern(state, "quote"), list);
}

sl_value sl_read(struct sl_interpreter_state *state, char *input)
{
        struct sl_reader *reader = sl_reader_new(input);
        sl_value val = sl_reader_read(state, reader);
        free(reader);

        return val;
}

void sl_init_reader()
{
        memzero(reader_macros, 256 * sizeof(reader_macro));

        reader_macros['('] = sl_reader_read_list;
        reader_macros['\''] = sl_reader_read_quote;
        reader_macros['"'] = sl_reader_read_string;
}
