static int madvise_hwpoison(int bhv, unsigned long start, unsigned long end)
{
	int ret = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	for (; start < end; start += PAGE_SIZE) {
		struct page *p;
		int ret = get_user_pages_fast(start, 1, 0, &p);
		if (ret != 1)
			return ret;
		if (bhv == MADV_SOFT_OFFLINE) {
			printk(KERN_INFO "Soft offlining page %lx at %lx\n",
				page_to_pfn(p), start);
			ret = soft_offline_page(p, MF_COUNT_INCREASED);
			if (ret)
				break;
			continue;
		}
		printk(KERN_INFO "Injecting memory failure for page %lx at %lx\n",
		       page_to_pfn(p), start);
		/* Ignore return value for now */
		memory_failure(page_to_pfn(p), 0, MF_COUNT_INCREASED);
	}
	return ret;
}