#include "shoelaces.h"

void sl_p(sl_value val)
{
        sl_value type = sl_type(val);

        if (sl_tType == type)
                sl_type_p(val);
        else if (sl_tInteger == type)
                sl_integer_p(val);
        else if (sl_tSymbol == type)
                sl_symbol_p(val);
        else if (sl_tBoolean == type)
                sl_boolean_p(val);
        else if (sl_tList == type)
                sl_list_p(val);
        else if (sl_tString == type)
                sl_string_p(val);
        else
                printf("#<%s>\n", SL_TYPE(sl_type(val))->name);
}
