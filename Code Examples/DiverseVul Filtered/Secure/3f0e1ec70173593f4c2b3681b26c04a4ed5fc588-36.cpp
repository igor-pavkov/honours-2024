static void __h2_deinit(void)
{
	pool_destroy(pool_head_h2s);
	pool_destroy(pool_head_h2c);
}