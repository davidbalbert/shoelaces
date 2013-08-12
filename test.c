#include <assert.h>

#include "shoelaces.h"

void test_read_integer()
{
        sl_value a = sl_read_string("1");
        sl_value b = sl_new_integer(1);

        assert(NUM2INT(a) == NUM2INT(b));
}

int main(int argc, char *argv[])
{
        sl_init();

        test_read_integer();
        return 0;
}
