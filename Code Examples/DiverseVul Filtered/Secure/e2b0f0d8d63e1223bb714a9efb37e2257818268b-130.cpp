static void avrcp_controller_remove(struct btd_service *service)
{
	control_unregister(service);
}