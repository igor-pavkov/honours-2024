static int __init hugetlb_default_setup(char *s)
{
	default_hstate_size = memparse(s, &s);
	return 1;
}