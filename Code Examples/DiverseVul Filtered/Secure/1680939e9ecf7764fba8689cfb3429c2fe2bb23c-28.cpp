static void __exit rpmsg_fini(void)
{
	unregister_virtio_driver(&virtio_ipc_driver);
}