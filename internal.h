sl_value boot_type_new(struct sl_interpreter_state *state, sl_value name);
sl_value boot_def_type(struct sl_interpreter_state *state, sl_value type);

void sl_gc_free_all(struct sl_interpreter_state *state);

int env_has_key(struct sl_interpreter_state *state, sl_value environment, sl_value name);
sl_value env_get(struct sl_interpreter_state *state, sl_value environment, sl_value name);

size_t sl_keyword_table_size(struct sl_interpreter_state *state);
