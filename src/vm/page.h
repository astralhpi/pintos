#include "lib/kernel/hash.h"
#include "filesys/off_t.h"
/* about supplementary(sub) table & table entry
they are all per-process resources
*/

enum page_status{
    ON_FRAME, /* On main memroy*/
    SWAPPED, /* On Swapped memory*/
    ON_DISK /* on disk */
};

/*
Subpage_table, of course, enters the thread's component. (because pagedir is component of threads)
We just define entry for that subpage_table(hash)
*/
struct sup_pagetable_entry
{
    struct hash_elem elem; /* for rotating */
    enum page_status status; /* page status */

    void * allocated_page; /* same as frame_entry in frame.h*/
    void * physical_memory;

    size_t swap_table_idx; /*if SWAPPED, index of sawp table that data saved*/

    /*component for load segments*/
    struct file *file;
    size_t page_read_bytes;
    size_t page_zero_bytes;
    off_t ofs;
    bool writable;
};

void sup_pagetable_create(struct thread *t);
void sup_pagetable_destroy(void);
bool sup_pagetable_set_page(struct thread *t, void* upage, void* ppage, enum page_status status);
bool sup_pagetable_clear_page (struct hash * sup_pagetable, void* upage);
struct sup_pagetable_entry * sup_lookup(struct hash * sup_pagetable, void* upage);