
void ax8swap_show_mem(void)
{
	int free = 0, total = 0, reserved = 0;
	int shared = 0, cached = 0, slab = 0, node, i;
	struct meminfo * mi = ax8swap_meminfo;

	printk("Mem-info:\n");
	show_free_areas();
	for_each_online_node(node) {
		pg_data_t *n = NODE_DATA(node);
		struct page *map = pgdat_page_nr(n, 0) - n->node_start_pfn;

		for_each_nodebank (i,mi,node) {
			struct membank *bank = &mi->bank[i];
			unsigned int pfn1, pfn2;
			struct page *page, *end;

			pfn1 = bank_pfn_start(bank);
			pfn2 = bank_pfn_end(bank);

			page = map + pfn1;
			end  = map + pfn2;

			do {
				total++;
				if (PageReserved(page))
					reserved++;
				else if (PageSwapCache(page))
					cached++;
				else if (PageSlab(page))
					slab++;
				else if (!page_count(page))
					free++;
				else
					shared += page_count(page) - 1;
				page++;
			} while (page < end);
		}
	}

	printk("%d pages of RAM\n", total);
	printk("%d free pages\n", free);
	printk("%d reserved pages\n", reserved);
	printk("%d slab pages\n", slab);
	printk("%d pages shared\n", shared);
	printk("%d pages swap cached\n", cached);
} 
