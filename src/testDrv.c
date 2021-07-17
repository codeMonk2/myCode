// Copyright (c) 2020 Arista Networks, Inc.  All rights reserved.
// Arista Networks, Inc. Confidential and Proprietary.

//#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>  /* printk() */
#include <linux/types.h>   /* size_t */
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/slab.h> /* kmalloc */
#include <linux/vmalloc.h> /* vmalloc */

unsigned long virtAddr;
struct page *pagePtr;

static void print_mem( struct task_struct *task ){
   struct mm_struct *mm;
   struct vm_area_struct *vma;
   int count = 0;
   mm = task->mm;
   printk( KERN_ERR "\nThis mm_struct has %d vmas.\n", mm->map_count );
   for ( vma = mm->mmap ; vma ; vma = vma->vm_next ) {
      printk( KERN_ERR "\nVma number %d: \n", ++count );
      printk( KERN_ERR "  Starts at 0x%lx, Ends at 0x%lx\n",
              vma->vm_start, vma->vm_end );
   }
   printk( KERN_ERR "\nCode  Segment start = 0x%lx, end = 0x%lx \n"
           "Data  Segment start = 0x%lx, end = 0x%lx\n"
           "Stack Segment start = 0x%lx\n",
           mm->start_code, mm->end_code,
           mm->start_data, mm->end_data,
           mm->start_stack );
}

static int mm_exp_load( void ){
   struct task_struct *task;
   int pid_mem = 1;
   for_each_process( task ) {
      if ( task->pid == pid_mem ) {
         printk( KERN_ERR "%s[%d]\n", task->comm, task->pid );
         print_mem( task );
      }
   }
   return 0;
}

int testDrv_init_module( void )
{
   int i = 0;
   int *arr = NULL;
   int *intPtr = NULL;
   printk( KERN_ERR "Entering module init\n" );
   virtAddr = __get_free_pages( GFP_KERNEL, 2 );
   if ( !virtAddr ) {
      printk( KERN_ERR "unable to allocation free pages\n" );
      return -1;
   }
   arr = ( int * )virtAddr;
   for( i = 0; i < 5; i++ ) {
      arr[i] = i;
   }
   for( i = 0; i < 5; i++ ) {
      printk( KERN_ERR "value is %p = %d\n", arr + i, arr[i] );
   }

   printk( KERN_ERR "page size is %lx page shift %d page offset %lx\n",
           PAGE_SIZE, PAGE_SHIFT, PAGE_OFFSET );

   printk( KERN_ERR "mem_map is at %p\n", &mem_map );

   pagePtr = alloc_page( GFP_KERNEL );
   if ( !pagePtr ) {
      printk( KERN_ERR "unable to alloc page\n" );
      free_pages( virtAddr, 2 );
      return -1;
   }

   printk( KERN_ERR "page addr %p ... actual pa is %lx page_ptr %p vtop %p\n",
           page_address( pagePtr ), __pa( page_address( pagePtr ) ),
           pagePtr, virt_to_page( page_address( pagePtr ) ) );

   intPtr = kmalloc( 4, GFP_KERNEL );
   if( !intPtr ) {
      free_pages( virtAddr, 2 );
      __free_page( pagePtr );
      printk( KERN_ERR "kmalloc failed\n" );
      return -1;
   }
   printk( KERN_ERR "malloced addr %p... actual pa is %lx\n",
           intPtr, __pa( intPtr ) );

   kfree( intPtr );

   printk( KERN_ERR "vm address start %lx end %lx\n",
           VMALLOC_START, VMALLOC_END );

   intPtr = vmalloc( 4 );
   if ( !intPtr ) {
      printk( KERN_ERR "vmalloc failed\n" );
      free_pages( virtAddr, 2 );
      __free_page( pagePtr );
      return -1;
   }
   printk( KERN_ERR "vmalloc addr is %p\n", intPtr );

   vfree( intPtr );

   printk( KERN_ERR "fn addr is %p\n", &testDrv_init_module );

   mm_exp_load();

   return 0;
}

void testDrv_cleanup_module( void )
{
   free_pages( virtAddr, 2 );
   __free_page( pagePtr );
   printk( KERN_ERR "Exiting module\n" );
   return;
}

module_init( testDrv_init_module );
module_exit( testDrv_cleanup_module );
