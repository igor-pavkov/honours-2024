void hugetlb_report_usage(struct seq_file *m, struct mm_struct *mm)
{
	seq_printf(m, "HugetlbPages:\t%8lu kB\n",
		   atomic_long_read(&mm->hugetlb_usage) << (PAGE_SHIFT - 10));
}