#include "shoelaces.h"

#define INITIAL_HEAP_CAPACITY 1000

static sl_value *find_free_slot();
static void increase_heap_size();
static void get_roots(sl_value **roots, size_t *roots_count);
static void mark(sl_value value);
static void unmark_all();
static void sweep_all();
static int marked(sl_value value);

static struct sl_heap *heap;

size_t
sl_gc_heap_size()
{
        return heap->size;
}

void
sl_gc_run()
{
        sl_value *roots;
        size_t root_count;

        unmark_all();

        get_roots(&roots, &root_count);

        for (size_t i = 0; i < root_count; i++) {
                mark(roots[i]);
        }

        free(roots);

        sweep_all();
}

static void
get_roots(sl_value **roots, size_t *roots_count)
{
        *roots = (sl_value *)sl_native_malloc(6 * sizeof(sl_value));
        (*roots)[0] = sl_tType;
        (*roots)[1] = sl_tSymbol;
        (*roots)[2] = sl_tInteger;
        (*roots)[3] = sl_tBoolean;
        (*roots)[4] = sl_tList;
        (*roots)[5] = sl_tString;

        *roots_count = 6;
}

/*
 * This assumes types and symbols are always roots.
 *
 * This should be rewritten with a queue.
 */
static void
mark(sl_value value)
{
        if (!value || marked(value)) {
                return;
        }

        SL_BASIC(value)->gc_mark = 1;

        sl_value type = sl_type(value);

        /* XXX: There's no need to mark the type of each object because all
         * types should be part of the root set. We might want to mark the type
         * field anyway though. I'm not sure. */

        if (sl_tType == type) {
                mark(SL_TYPE(value)->name);
        } else if (sl_tSymbol == type) {
                mark(SL_SYMBOL(value)->name);
        } else if (sl_tList == type) {
                mark(SL_LIST(value)->first);
                mark(SL_LIST(value)->rest);
        }
}

static void
unmark_all()
{
        for (size_t i = 0; i < heap->capacity; i++) {
                if (heap->slots[i]) {
                        SL_BASIC(heap->slots[i])->gc_mark = 0;
                }
        }
}

static void
sweep_all()
{
        for (size_t i = 0; i < heap->capacity; i++) {
                if (heap->slots[i] && !marked(heap->slots[i])) {
                        sl_dealloc(heap->slots[i]);
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
sl_gc_alloc(size_t size)
{
        sl_value *slot = find_free_slot();
        if (!slot) {
                sl_gc_run();
                slot = find_free_slot();

                if (!slot) {
                        increase_heap_size();
                        return sl_gc_alloc(size);
                }
        }

        *slot = (sl_value)sl_native_malloc(size);
        heap->size++;

        return *slot;
}

static sl_value *
find_free_slot()
{
        for (size_t i = heap->capacity; i > 0; i--) {
                if (!heap->slots[i - 1]) {
                        return &heap->slots[i - 1];
                }
        }

        return NULL;
}

static void
increase_heap_size()
{
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
sl_init_gc()
{
        heap = (struct sl_heap *)sl_native_malloc(sizeof(struct sl_heap));

        heap->capacity = INITIAL_HEAP_CAPACITY;
        heap->size = 0;
        heap->slots = (sl_value *)sl_native_malloc(heap->capacity * sizeof(sl_value));
        memzero(heap->slots, heap->capacity * sizeof(sl_value));
}
