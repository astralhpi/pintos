#include <stdio.h>
#include <hash.h>
#include "threads/thread.h"
#include "threads/synch.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "vm/frame.h"

static struct lock frame_table_lock;
static struct hash frame_table;

static unsigned
frame_hash_func (const struct hash_elem *elem, void *aux UNUSED)
{
  struct frame_table_entry *frame_entry = hash_entry (elem, struct frame_table_entry, h_elem);
  return hash_bytes((void *) &frame_entry->frame, sizeof frame_entry->frame);
}
static bool
frame_less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED)
{
  struct frame_table_entry *a_entry = hash_entry (a, struct frame_table_entry, h_elem);
  struct frame_table_entry *b_entry = hash_entry (b, struct frame_table_entry, h_elem);
  return a_entry->frame < b_entry->frame;
}

/*
 * Initialize frame table
 */
void 
frame_init (void)
{
  lock_init (&frame_table_lock);
  hash_init (&frame_table, frame_hash_func, frame_less_func, NULL);
}


/* 
 * Make a new frame table entry for addr.
 */
void *
allocate_frame (enum palloc_flags flags, void *addr UNUSED)
{
  ASSERT (flags & PAL_USER)

  uint8_t *kpage = palloc_get_page (flags);
  // Allocation failed
  if (kpage == NULL) {
    return NULL;
  }

  struct frame_table_entry *fte = malloc (sizeof (struct frame_table_entry));
  // TODO : Check validity ?
  fte->owner = thread_current ();
  fte->frame = (void *) vtop (kpage);
  // TODO: supplementary table ?
  fte->page = kpage; // TODO: vaddr?

  lock_acquire (&frame_table_lock);
  hash_insert (&frame_table, &fte->h_elem);
  lock_release (&frame_table_lock);

  return kpage;
}

/*
 * Free a frame table entry from kpage.
 * kpage must be kernel virtual address.
 */
void
free_frame (void *kpage)
{
  struct frame_table_entry *fte;
  struct frame_table_entry *tmp_fte;

  // TODO: Check if kpage valid ?
  tmp_fte = (struct frame_table_entry *) malloc (sizeof (struct frame_table_entry));

  tmp_fte->frame = (void *) vtop (kpage);

  struct hash_elem *h_elem = hash_find (&frame_table, &tmp_fte->h_elem);

  free (tmp_fte);
  if (h_elem == NULL) {
    // TODO: Frame not found!? handle it
    return;
  }

  fte = hash_entry (h_elem, struct frame_table_entry, h_elem);

  lock_acquire (&frame_table_lock);
  hash_delete (&frame_table, &fte->h_elem);
  lock_release (&frame_table_lock);

  palloc_free_page (fte->page);
  free (fte);
}