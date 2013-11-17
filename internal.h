sl_value boot_type_new(struct sl_interpreter_state *state, sl_value name);
sl_value boot_abstract_type_new(struct sl_interpreter_state *state, sl_value name, sl_value type_variables);
sl_value boot_def_type(struct sl_interpreter_state *state, sl_value type);

void sl_gc_free_all(struct sl_interpreter_state *state);
void sl_free_keep_list(struct sl_keep_list *start, struct sl_keep_list *end);
void sl_gc_enable(struct sl_interpreter_state *state);
void sl_gc_disable(struct sl_interpreter_state *state);

int sl_env_has_key(struct sl_interpreter_state *state, sl_value environment, sl_value name);
sl_value sl_env_get(struct sl_interpreter_state *state, sl_value environment, sl_value name);

size_t sl_keyword_table_size(struct sl_interpreter_state *state);

sl_value sl_fn(struct sl_interpreter_state *state, sl_value signature, sl_value bodies, sl_value environment);
