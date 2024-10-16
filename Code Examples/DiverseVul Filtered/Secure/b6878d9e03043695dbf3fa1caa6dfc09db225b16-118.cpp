void mddev_unlock(struct mddev *mddev)
{
	if (mddev->to_remove) {
		/* These cannot be removed under reconfig_mutex as
		 * an access to the files will try to take reconfig_mutex
		 * while holding the file unremovable, which leads to
		 * a deadlock.
		 * So hold set sysfs_active while the remove in happeing,
		 * and anything else which might set ->to_remove or my
		 * otherwise change the sysfs namespace will fail with
		 * -EBUSY if sysfs_active is still set.
		 * We set sysfs_active under reconfig_mutex and elsewhere
		 * test it under the same mutex to ensure its correct value
		 * is seen.
		 */
		struct attribute_group *to_remove = mddev->to_remove;
		mddev->to_remove = NULL;
		mddev->sysfs_active = 1;
		mutex_unlock(&mddev->reconfig_mutex);

		if (mddev->kobj.sd) {
			if (to_remove != &md_redundancy_group)
				sysfs_remove_group(&mddev->kobj, to_remove);
			if (mddev->pers == NULL ||
			    mddev->pers->sync_request == NULL) {
				sysfs_remove_group(&mddev->kobj, &md_redundancy_group);
				if (mddev->sysfs_action)
					sysfs_put(mddev->sysfs_action);
				mddev->sysfs_action = NULL;
			}
		}
		mddev->sysfs_active = 0;
	} else
		mutex_unlock(&mddev->reconfig_mutex);

	/* As we've dropped the mutex we need a spinlock to
	 * make sure the thread doesn't disappear
	 */
	spin_lock(&pers_lock);
	md_wakeup_thread(mddev->thread);
	spin_unlock(&pers_lock);
}