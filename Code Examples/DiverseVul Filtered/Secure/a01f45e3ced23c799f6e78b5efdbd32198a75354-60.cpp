static inline int32_t h2c_get_next_sid(const struct h2c *h2c)
{
	int32_t id = (h2c->max_id + 1) | 1;
	if (id & 0x80000000U)
		id = -1;
	return id;
}