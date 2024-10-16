static ssize_t new_offset_show(struct md_rdev *rdev, char *page)
{
	return sprintf(page, "%llu\n",
		       (unsigned long long)rdev->new_data_offset);
}