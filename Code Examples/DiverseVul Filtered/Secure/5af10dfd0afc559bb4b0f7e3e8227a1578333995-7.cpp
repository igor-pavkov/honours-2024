static ssize_t nr_overcommit_hugepages_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	int err;
	unsigned long input;
	struct hstate *h = kobj_to_hstate(kobj, NULL);

	if (hstate_is_gigantic(h))
		return -EINVAL;

	err = kstrtoul(buf, 10, &input);
	if (err)
		return err;

	spin_lock(&hugetlb_lock);
	h->nr_overcommit_huge_pages = input;
	spin_unlock(&hugetlb_lock);

	return count;
}