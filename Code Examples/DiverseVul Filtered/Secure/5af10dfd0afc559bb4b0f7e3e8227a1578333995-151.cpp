int hugetlb_sysctl_handler(struct ctl_table *table, int write,
			  void __user *buffer, size_t *length, loff_t *ppos)
{

	return hugetlb_sysctl_handler_common(false, table, write,
							buffer, length, ppos);
}