static int __init rpmsg_init(void)
{
	int ret;

	ret = register_virtio_driver(&virtio_ipc_driver);
	if (ret)
		pr_err("failed to register virtio driver: %d\n", ret);

	return ret;
}