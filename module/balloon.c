/*                                                                                  
 * Simple Balloon Driver                                                        
 *                                                                                  
 * Copyright (c) 2018 Computer Systems Laboratory, Sungkyunkwan University.         
 * http://csl.skku.edu                                                              
 *                                                                                  
 * This program is free software; you can redistribute it and/or                    
 * modify it under the terms of the GNU General Public License                      
 * as published by the Free Software Foundation; either version 2                   
 * of the License, or (at your option) any later version.                           
 *                                                                                  
 * This program is distributed in the hope that it will be useful,                  
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    
 * GNU General Public License for more details.                                     
 *                                                                                  
 * You should have received a copy of the GNU General Public License                
 * along with this program; if not, write to the Free Software                      
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */ 

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/list.h>

static int nr_pages = 0; 
module_param(nr_pages, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(nr_pages, "# of pages to alloc");

static struct list_head page_list;

static void balloon_pages_nid(int node_nr_pages, int nid)
{
	int nr_pages_remain = node_nr_pages;
	int order = MAX_ORDER - 1;

	printk(KERN_INFO "balloon: allocating %d pages in node %d\n", node_nr_pages, nid);

	if ( node_nr_pages == 0 )
		goto exit;

	while ( order > 0  && 1 << order > node_nr_pages )
		order -= 1;

	while (nr_pages_remain > 0 ) {
		struct page* page = alloc_pages_node(nid, GFP_HIGHUSER_MOVABLE, order);
		if ( page == NULL ) {
			if ( order == 0 ) {
				printk(KERN_INFO "cannot allocate pages order = %d\n", order);
				goto exit;
			}
			order -= 1;
			continue;
		}

		page->private = order;
		list_add_tail(&page->lru, &page_list);
		nr_pages_remain -= (1 << order);
	}
exit:
	printk(KERN_ERR "balloon: remaining pages = %d in node %d\n", nr_pages_remain, nid);
}


static int __init balloon_init(void)
{
	int nid;
	int nr_pages_per_node = nr_pages / num_online_nodes();

	INIT_LIST_HEAD(&page_list);

	printk(KERN_INFO "balloon: init with nr_pages = %d online nodes = %d nr_pages_per_node = %d\n", nr_pages, num_online_nodes(), nr_pages_per_node);

	for_each_online_node(nid) {
		balloon_pages_nid(nr_pages_per_node, nid);
	}

	return 0;
}

static void __exit balloon_exit(void)
{
	struct list_head* head, *next;
	int nr_pages_freed = 0;
	list_for_each_safe(head, next, &page_list) {
		struct page* page = list_entry(head, struct page, lru);
		nr_pages_freed += 1 << page->private;
		__free_pages(page, page->private);
	}

	printk(KERN_INFO "balloon: cleaning-up module nr_pages_freed = %d\n", nr_pages_freed);
}


MODULE_DESCRIPTION("Simple Balloon Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jinkyu Jeong <jinkyu@csl.skku.edu>");
MODULE_VERSION("0.1");

module_init(balloon_init);
module_exit(balloon_exit);
