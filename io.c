#include "shoelaces.h"

sl_value sl_generic_inspect(sl_value val)
{
        sl_value type = sl_type(val);
        sl_value str;
        char *inspect_string = sl_native_malloc((4 + NUM2INT(sl_size(SL_TYPE(type)->name))) * sizeof(char));

        sprintf(inspect_string, "#<%s>", sl_string_cstring(SL_TYPE(type)->name));

        str = sl_string_new(inspect_string);
        free(inspect_string);

        return str;
}

sl_value sl_inspect(sl_value val)
{
        sl_value type = sl_type(val);
        sl_value str;

        if (sl_tType == type) {
                str = sl_type_inspect(val);
        } else if (sl_tInteger == type) {
                str = sl_integer_inspect(val);
        } else if (sl_tSymbol == type) {
                str = sl_symbol_inspect(val);
        } else if (sl_tBoolean == type) {
                str = sl_boolean_inspect(val);
        } else if (sl_tList == type) {
                str = sl_list_inspect(val);
        } else if (sl_tString == type) {
                str = sl_string_inspect(val);
        } else {
                str = sl_generic_inspect(val);
        }

        return str;
}

void sl_p(sl_value val)
{
        printf("%s\n", sl_string_cstring(sl_inspect(val)));
}
