static void smtc_unmap_mmio(struct smtcfb_info *sfb)
{
	if (sfb && smtc_regbaseaddress)
		smtc_regbaseaddress = NULL;
}