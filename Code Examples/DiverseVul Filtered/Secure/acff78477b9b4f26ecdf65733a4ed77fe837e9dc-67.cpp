static int nested_check_guest_non_reg_state(struct vmcs12 *vmcs12)
{
	if (vmcs12->guest_activity_state != GUEST_ACTIVITY_ACTIVE &&
	    vmcs12->guest_activity_state != GUEST_ACTIVITY_HLT)
		return -EINVAL;

	return 0;
}