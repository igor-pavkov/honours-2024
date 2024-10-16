static void avrcp_exit(void)
{
	btd_profile_unregister(&avrcp_controller_profile);
	btd_profile_unregister(&avrcp_target_profile);
}