static void uvesafb_exit(void)
{
	struct uvesafb_ktask *task;

	if (v86d_started) {
		task = uvesafb_prep();
		if (task) {
			task->t.flags = TF_EXIT;
			uvesafb_exec(task);
			uvesafb_free(task);
		}
	}

	cn_del_callback(&uvesafb_cn_id);
	driver_remove_file(&uvesafb_driver.driver, &driver_attr_v86d);
	platform_device_unregister(uvesafb_device);
	platform_driver_unregister(&uvesafb_driver);
}