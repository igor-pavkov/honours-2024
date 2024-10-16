static void __h2_init(void)
{
	alpn_register_mux(&alpn_mux_h2);
	cfg_register_keywords(&cfg_kws);
	hap_register_post_deinit(__h2_deinit);
	pool_head_h2c = create_pool("h2c", sizeof(struct h2c), MEM_F_SHARED);
	pool_head_h2s = create_pool("h2s", sizeof(struct h2s), MEM_F_SHARED);
}