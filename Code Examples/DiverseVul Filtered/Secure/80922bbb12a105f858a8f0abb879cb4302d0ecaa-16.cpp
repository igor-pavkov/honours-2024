static void econet_hw_initialise(void)
{
	dev_add_pack(&econet_packet_type);
}