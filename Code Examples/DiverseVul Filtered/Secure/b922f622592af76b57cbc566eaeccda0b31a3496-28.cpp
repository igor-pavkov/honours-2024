bool hw_atl_utils_ver_match(u32 ver_expected, u32 ver_actual)
{
	const u32 dw_major_mask = 0xff000000U;
	const u32 dw_minor_mask = 0x00ffffffU;
	bool ver_match;

	ver_match = (dw_major_mask & (ver_expected ^ ver_actual)) ? false : true;
	if (!ver_match)
		goto err_exit;
	ver_match = ((dw_minor_mask & ver_expected) > (dw_minor_mask & ver_actual)) ?
		false : true;

err_exit:
	return ver_match;
}