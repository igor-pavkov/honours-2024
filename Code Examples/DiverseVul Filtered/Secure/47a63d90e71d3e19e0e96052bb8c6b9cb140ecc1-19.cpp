static int rsync_xal_compare_names(const void *x1, const void *x2)
{
	const rsync_xa *xa1 = x1;
	const rsync_xa *xa2 = x2;
	return strcmp(xa1->name, xa2->name);
}