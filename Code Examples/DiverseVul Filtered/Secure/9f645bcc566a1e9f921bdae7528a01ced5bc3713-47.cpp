static int uvesafb_init(void)
{
	int err;

#ifndef MODULE
	char *option = NULL;

	if (fb_get_options("uvesafb", &option))
		return -ENODEV;
	uvesafb_setup(option);
#endif
	err = cn_add_callback(&uvesafb_cn_id, "uvesafb", uvesafb_cn_callback);
	if (err)
		return err;

	err = platform_driver_register(&uvesafb_driver);

	if (!err) {
		uvesafb_device = platform_device_alloc("uvesafb", 0);
		if (uvesafb_device)
			err = platform_device_add(uvesafb_device);
		else
			err = -ENOMEM;

		if (err) {
			platform_device_put(uvesafb_device);
			platform_driver_unregister(&uvesafb_driver);
			cn_del_callback(&uvesafb_cn_id);
			return err;
		}

		err = driver_create_file(&uvesafb_driver.driver,
				&driver_attr_v86d);
		if (err) {
			pr_warn("failed to register attributes\n");
			err = 0;
		}
	}
	return err;
}