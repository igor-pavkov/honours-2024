static int esil_avr_init(RAnalEsil *esil) {
	if (!esil) {
		return false;
	}
	desctx.round = 0;
	r_anal_esil_set_op (esil, "des", avr_custom_des);
	r_anal_esil_set_op (esil, "SPM_PAGE_ERASE", avr_custom_spm_page_erase);
	r_anal_esil_set_op (esil, "SPM_PAGE_FILL", avr_custom_spm_page_fill);
	r_anal_esil_set_op (esil, "SPM_PAGE_WRITE", avr_custom_spm_page_write);
	esil->cb.hook_reg_write = esil_avr_hook_reg_write;

	return true;
}