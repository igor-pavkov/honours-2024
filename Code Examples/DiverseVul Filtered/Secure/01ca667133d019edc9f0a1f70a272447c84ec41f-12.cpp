void fm10k_tx_timeout_reset(struct fm10k_intfc *interface)
{
	/* Do the reset outside of interrupt context */
	if (!test_bit(__FM10K_DOWN, interface->state)) {
		interface->tx_timeout_count++;
		set_bit(FM10K_FLAG_RESET_REQUESTED, interface->flags);
		fm10k_service_event_schedule(interface);
	}
}