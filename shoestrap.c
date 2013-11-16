#include <stdio.h>
#include <stdlib.h>

#include "shoelaces.h"
#include "internal.h"

#define MAX_INPUT_SIZE 8192

int
main(int argc, char *argv[])
{
        printf("Welcome to Shoestrap\n");

        struct sl_interpreter_state *state = sl_init();
        char *input_string = malloc(MAX_INPUT_SIZE * sizeof(char));
        sl_value in, out;

        while (1) {
                printf(">> ");
                fflush(stdin);
                input_string = fgets(input_string, MAX_INPUT_SIZE, stdin);

                if (input_string == NULL) {
                        break;
                }

                struct sl_keep_list *old = state->keep_list;

                in = sl_read(state, input_string);
                out = sl_eval(state, in, state->global_env);

                sl_free_keep_list(state->keep_list, old);
                state->keep_list = old;

                sl_p(state, out);
        }

        free(input_string);
        sl_destroy(state);
        return 0;
}
