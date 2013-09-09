#include "shoelaces.h"

#define INITIAL_HEAP_CAPACITY 1000

static sl_value *find_free_slot(struct sl_interpreter_state *state);
static void increase_heap_size(struct sl_interpreter_state *state);
static void get_roots(struct sl_interpreter_state *state, sl_value **roots, size_t *roots_count);
static void mark(struct sl_interpreter_state *state, sl_value value);
static void unmark_all(struct sl_interpreter_state *state);
static void sweep_all(struct sl_interpreter_state *state);
static int marked(sl_value value);

size_t
sl_gc_heap_size(struct sl_interpreter_state *state)
{
        return state->heap->size;
}

void
sl_gc_run(struct sl_interpreter_state *state)
{
        sl_value *roots;
        size_t root_count;

        unmark_all(state);

        get_roots(state, &roots, &root_count);

        for (size_t i = 0; i < root_count; i++) {
                mark(state, roots[i]);
        }

        free(roots);

        sweep_all(state);
}

static void
get_roots(struct sl_interpreter_state *state, sl_value **roots, size_t *roots_count)
{
        size_t count = 3 + sl_symbol_table_size(state);
        *roots = (sl_value *)sl_native_malloc(count * sizeof(sl_value));
        (*roots)[0] = state->global_env;
        (*roots)[1] = state->sl_true;
        (*roots)[2] = state->sl_false;

        khash_t(str) *symbol_table = state->symbol_table;
        khiter_t iter;
        int i = 3;

        for (iter = kh_begin(symbol_table); iter != kh_end(symbol_table); ++iter) {
                if (kh_exist(symbol_table, iter)) {
                        (*roots)[i] = kh_value(symbol_table, iter);
                        i++;
                }
        }

        assert(i == count);
        *roots_count = count;
}

/*
 * This assumes types and symbols are always roots.
 *
 * This should be rewritten with a queue.
 */
static void
mark(struct sl_interpreter_state *state, sl_value value)
{
        /* TODO: when we move to tagged values, we will have to
         * check for value == Qundefined rather than !value */
        if (!value || marked(value)) {
                return;
        }

        SL_BASIC(value)->gc_mark = 1;

        sl_value type = sl_type(value);

        /* XXX: There's no need to mark the type of each object because all
         * types should be part of the root set. We might want to mark the type
         * field anyway though. I'm not sure. */

        if (state->tType == type) {
                mark(state, SL_TYPE(value)->name);
        } else if (state->tSymbol == type) {
                mark(state, SL_SYMBOL(value)->name);
        } else if (state->tList == type) {
                mark(state, SL_LIST(value)->first);
                mark(state, SL_LIST(value)->rest);
        } else if (state->tFunction == type) {
                mark(state, SL_FUNCTION(value)->name);
                mark(state, SL_FUNCTION(value)->method_table);
        } else if (state->tMethodTable == type) {
                mark(state, SL_METHOD_TABLE(value)->method);
                mark(state, SL_METHOD_TABLE(value)->method_map);
        } else if (state->tMethod == type) {
                mark(state, SL_METHOD(value)->signature);
                mark(state, SL_METHOD(value)->function);
        }
}

static void
unmark_all(struct sl_interpreter_state *state)
{
        struct sl_heap *heap = state->heap;

        for (size_t i = 0; i < heap->capacity; i++) {
                if (heap->slots[i]) {
                        SL_BASIC(heap->slots[i])->gc_mark = 0;
                }
        }
}

static void
sweep_all(struct sl_interpreter_state *state)
{
        struct sl_heap *heap = state->heap;

        for (size_t i = 0; i < heap->capacity; i++) {
                if (heap->slots[i] && !marked(heap->slots[i])) {
                        sl_dealloc(state, heap->slots[i]);
                        heap->slots[i] = NULL;
                        heap->size--;
                }
        }
}

int
marked(sl_value value)
{
        return SL_BASIC(value)->gc_mark;
}

sl_value
sl_gc_alloc(struct sl_interpreter_state *state, size_t size)
{
        sl_value *slot = find_free_slot(state);
        if (!slot) {
                sl_gc_run(state);
                slot = find_free_slot(state);

                if (!slot) {
                        increase_heap_size(state);
                        return sl_gc_alloc(state, size);
                }
        }

        *slot = (sl_value)sl_native_malloc(size);
        state->heap->size++;

        return *slot;
}

void
sl_dealloc(struct sl_interpreter_state *state, sl_value value)
{
        sl_value type = sl_type(value);

        if (type == state->tString) {
                free(SL_STRING(value)->value);
        }

        free(value);
}

void
sl_gc_free_all(struct sl_interpreter_state *state)
{
        struct sl_heap *heap = state->heap;

        for (size_t i = 0; i < heap->capacity; i++) {
                if (heap->slots[i]) {
                        sl_dealloc(state, heap->slots[i]);
                        heap->slots[i] = NULL;
                        heap->size--;
                }
        }

        assert(heap->size == 0);
}

static sl_value *
find_free_slot(struct sl_interpreter_state *state)
{
        struct sl_heap *heap = state->heap;

        for (size_t i = heap->capacity; i > 0; i--) {
                if (!heap->slots[i - 1]) {
                        return &heap->slots[i - 1];
                }
        }

        return NULL;
}

static void
increase_heap_size(struct sl_interpreter_state *state)
{
        struct sl_heap *heap = state->heap;

        heap->capacity *= 2;
        heap->slots = realloc(heap->slots, heap->capacity * sizeof(sl_value));

        if (!heap->slots) {
                fprintf(stderr, "Out of memory!\n");
                abort();
        }

        memzero(heap->slots + heap->capacity / 2, heap->capacity / 2);
}

void *
sl_native_malloc(size_t size)
{
        void *p = malloc(size);
        if (!p) {
                fprintf(stderr, "Out of memory!\n");
                abort();
        }

        /* It's obvious what this next line does, but perhaps it would be good
         * to explain the motivation. I ran into a bug with an early version of
         * the garbage collector. I was allocating many objects and not
         * referencing any of them from the root set. Once I allocated enough
         * objects to fill up the heap and trigger the garbage collector, some
         * of the objects I subsequently allocated stuck around, even when the
         * GC was forced to run. After some thinking I realized that I was
         * assuming that all of the marked objects were marked during a call to
         * mark(). My assumption was incorrect because malloc doesn't make any
         * guarantees about the state of the memory allocated (calloc, on the
         * other hand, zeroes the memory before returning the pointer, but
         * we're not using it). Some objects were being returned with their
         * mark bit already set, and because I don't touch the mark bit in
         * object initialization, this information was persisting to garbage
         * collection and the objects were sticking around.
         *
         * Thinking about this bug revealed another bug: mark bits weren't
         * being cleared before each GC run. This meant that once an object was
         * marked, it would stick around forever. Had I caught this bug earlier
         * and cleared all the mark bits before each GC run, I might not have
         * realized that I was returning uninitialized data from this
         * function.*/
        memzero(p, size);

        return p;
}

void
sl_init_gc(struct sl_interpreter_state *state)
{
        struct sl_heap *heap;
        state->heap = heap = (struct sl_heap *)sl_native_malloc(sizeof(struct sl_heap));

        heap->capacity = INITIAL_HEAP_CAPACITY;
        heap->size = 0;
        heap->slots = (sl_value *)sl_native_malloc(heap->capacity * sizeof(sl_value));
        memzero(heap->slots, heap->capacity * sizeof(sl_value));
}
