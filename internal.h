sl_value boot_type_new(struct sl_interpreter_state *state, sl_value name);
sl_value boot_def_type(struct sl_interpreter_state *state, sl_value type);

void sl_gc_free_all(struct sl_interpreter_state *state);
