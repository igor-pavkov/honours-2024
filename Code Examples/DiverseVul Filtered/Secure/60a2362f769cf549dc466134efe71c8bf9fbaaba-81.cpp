static void _regulator_put(struct regulator *regulator)
{
	struct regulator_dev *rdev;

	if (regulator == NULL || IS_ERR(regulator))
		return;

	rdev = regulator->rdev;

	debugfs_remove_recursive(regulator->debugfs);

	/* remove any sysfs entries */
	if (regulator->dev)
		sysfs_remove_link(&rdev->dev.kobj, regulator->supply_name);
	kfree(regulator->supply_name);
	list_del(&regulator->list);
	kfree(regulator);

	rdev->open_count--;
	rdev->exclusive = 0;

	module_put(rdev->owner);
}