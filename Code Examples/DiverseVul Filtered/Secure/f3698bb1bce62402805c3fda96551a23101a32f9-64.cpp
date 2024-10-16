void gf_fm_request_call(u32 type, u32 param, int *value) {
	if (fm_cbk)
		fm_cbk(fm_cbk_obj, type, param, value);
}