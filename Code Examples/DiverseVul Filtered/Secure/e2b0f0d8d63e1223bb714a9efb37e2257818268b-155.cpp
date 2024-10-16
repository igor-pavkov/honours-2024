static int avrcp_init(void)
{
	btd_profile_register(&avrcp_controller_profile);
	btd_profile_register(&avrcp_target_profile);

	populate_default_features();

	return 0;
}