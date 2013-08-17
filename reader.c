#include "shoelaces.h"

struct sl_reader
{
        char *input;
        char *start_position;
        char *end_position;
};

static struct sl_reader *sl_new_reader(char *input)
{
        struct sl_reader *reader = sl_alloc(struct sl_reader);
        reader->input = input;
        reader->start_position = input;
        reader->end_position = input;
        return reader;
}

static int sl_reader_at_end_of_input(struct sl_reader *reader)
{
        return *reader->end_position == '\0';
}

static int sl_reader_next_char_is_whitespace(struct sl_reader *reader)
{
        int ch = *reader->end_position;

        return isspace(ch);
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

static void sl_reader_skip_one(struct sl_reader *reader)
{
        reader->start_position++;
        reader->end_position = reader->start_position;
}

static void sl_reader_advance_one(struct sl_reader *reader)
{
        reader->end_position++;
}

static int sl_reader_peek(struct sl_reader *reader)
{
        return *reader->end_position;
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
        } else if (strcmp(token, "nil") == 0) {
                return sl_nil;
        } else if (strcmp(token, "()") == 0) {
                return sl_nil;
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
                        number = sl_new_integer(strtol(token, NULL, 10));
                        free(token);
                        return number;
                } else {
                        sl_reader_error(reader, "Expecting digit, but got %c", sl_reader_peek(reader));
                }
        }
}

sl_value sl_read(struct sl_interpreter_state *state, char *input)
{
        struct sl_reader *reader = sl_new_reader(input);
        char ch;

        while (1) {
                while (sl_reader_next_char_is_whitespace(reader)) {
                        sl_reader_skip_one(reader);
                }

                if (sl_reader_at_end_of_input(reader)) {
                        return NULL;
                }

                if (sl_reader_next_char_is_digit(reader)) {
                        return sl_reader_read_integer(reader);
                }

                return sl_reader_parse_token(state, sl_reader_read_token(reader));
        }
}
