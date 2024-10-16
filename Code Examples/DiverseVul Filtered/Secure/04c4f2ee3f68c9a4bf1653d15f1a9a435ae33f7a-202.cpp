static bool vmx_check_apicv_inhibit_reasons(ulong bit)
{
	ulong supported = BIT(APICV_INHIBIT_REASON_DISABLE) |
			  BIT(APICV_INHIBIT_REASON_HYPERV);

	return supported & BIT(bit);
}