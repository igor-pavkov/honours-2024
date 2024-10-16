static void avrcp_target_remove(struct btd_service *service)
{
	control_unregister(service);
}