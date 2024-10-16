static inline unsigned long get_desc_limit(const struct desc_struct *desc)
{
	return desc->limit0 | (desc->limit << 16);
}