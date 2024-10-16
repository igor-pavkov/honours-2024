static int __init vhost_vdpa_init(void)
{
	int r;

	r = alloc_chrdev_region(&vhost_vdpa_major, 0, VHOST_VDPA_DEV_MAX,
				"vhost-vdpa");
	if (r)
		goto err_alloc_chrdev;

	r = vdpa_register_driver(&vhost_vdpa_driver);
	if (r)
		goto err_vdpa_register_driver;

	return 0;

err_vdpa_register_driver:
	unregister_chrdev_region(vhost_vdpa_major, VHOST_VDPA_DEV_MAX);
err_alloc_chrdev:
	return r;
}