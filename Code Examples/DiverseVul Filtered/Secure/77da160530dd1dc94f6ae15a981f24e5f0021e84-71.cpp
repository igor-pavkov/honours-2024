static struct kobject *exact_match(dev_t devt, int *partno, void *data)
{
	struct gendisk *p = data;

	return &disk_to_dev(p)->kobj;
}