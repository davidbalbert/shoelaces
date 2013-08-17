#include "shoelaces.h"

sl_value sl_tList;
sl_value sl_nil;

void sl_init_list()
{
        sl_tList = sl_new_type("List");
        sl_nil = NULL;
}
