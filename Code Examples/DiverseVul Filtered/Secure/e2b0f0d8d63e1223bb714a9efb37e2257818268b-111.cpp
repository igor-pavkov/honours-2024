static int avrcp_controller_probe(struct btd_service *service)
{
	struct btd_device *dev = btd_service_get_device(service);

	DBG("path %s", device_get_path(dev));

	return control_init_remote(service);
}