static int rds_pin_pages(unsigned long user_addr, unsigned int nr_pages,
			struct page **pages, int write)
{
	int ret;

	ret = get_user_pages_fast(user_addr, nr_pages, write, pages);

	if (ret >= 0 && ret < nr_pages) {
		while (ret--)
			put_page(pages[ret]);
		ret = -EFAULT;
	}

	return ret;
}