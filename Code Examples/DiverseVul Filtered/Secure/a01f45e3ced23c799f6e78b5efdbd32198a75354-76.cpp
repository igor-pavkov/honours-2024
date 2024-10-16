static inline int h2_has_too_many_cs(const struct h2c *h2c)
{
	return h2c->nb_cs > h2_settings_max_concurrent_streams;
}