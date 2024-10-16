void gf_fm_request_set_callback(void *cbk_obj, fm_callback_func cbk_func) {
	fm_cbk = cbk_func;
	fm_cbk_obj = cbk_obj;
}