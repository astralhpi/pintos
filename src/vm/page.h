#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <hash.h>
#include "filesys/off_t.h"
#include "threads/synch.h"
#include "threads/palloc.h"

/*
 * Do supplementary page tables need supplementary page dir table?
 * Maybe yes or maybe not.
 * As sup_pages don't need to be grouped as the same way pages are
 * grouped, supplementary page tables don't need sup_page_dir
struct sup_pagedir_table
{
    struct hash sup_page_table;
    struct lock sup_page_table_lock;
};

uint32_t * sup_pagedir_create (void);
struct hash* sup_page_create (void);

uint32_t *
sup_pagedir_create (void)
{
  struct sup_pagedir_table *spdt = malloc (sizeof (struct sup_pagedir_table));
  return spdt;
}

void
sup_pagedir_destroy (void *pagedir)
{
  // TODO: destroy sup pagedir table
}
*/

enum
page_source
{
    FILE_SYS,
    SWAP,
    ALL_ZERO,
};

struct sup_page_table_entry
{
	uint32_t *user_vaddr;
	uint32_t *frame;

	// swap_key TODO: swap key if swapped

	uint64_t access_time;

	struct hash_elem h_elem;

	bool on_frame;
	bool dirty;
	bool accessed;

	enum page_source source;

	/* File */ // TODO: Can we place these information somewhere else?
  struct file * file;
  off_t file_offset;
  uint32_t file_page_read_bytes, file_page_zero_bytes;
  bool file_writable;
};

struct hash* sup_page_create (void);
void sup_page_destroy (struct hash *sup_page_table);

bool sup_page_install_frame (struct hash *sup_page_table, void *upage, void *kpage);
bool sup_page_table_has_entry (struct hash *sup_page_table, void *addr);

bool sup_page_install_zero_page (void *vaddr);
bool sup_page_reserve_segment (void *vaddr, struct file * file, off_t offset, uint32_t page_read_bytes, uint32_t page_zero_bytes, bool writable);

bool sup_page_load_page (void *vaddr);

#endif /* vm/page.h */