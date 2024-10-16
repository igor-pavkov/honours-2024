static void __exit vhost_vdpa_exit(void)
{
	vdpa_unregister_driver(&vhost_vdpa_driver);
	unregister_chrdev_region(vhost_vdpa_major, VHOST_VDPA_DEV_MAX);
}