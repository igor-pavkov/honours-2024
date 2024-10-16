static int avrcp_connect(struct btd_service *service)
{
	struct btd_device *dev = btd_service_get_device(service);
	const char *path = device_get_path(dev);

	DBG("path %s", path);

	return control_connect(service);
}