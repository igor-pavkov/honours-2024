static int part_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	struct block_device *part = dev_to_bdev(dev);

	add_uevent_var(env, "PARTN=%u", part->bd_partno);
	if (part->bd_meta_info && part->bd_meta_info->volname[0])
		add_uevent_var(env, "PARTNAME=%s", part->bd_meta_info->volname);
	return 0;
}