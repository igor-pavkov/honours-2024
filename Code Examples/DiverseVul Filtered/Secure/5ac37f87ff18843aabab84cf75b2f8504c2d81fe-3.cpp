static inline unsigned long get_desc_base(const struct desc_struct *desc)
{
	return desc->base0 | ((desc->base1) << 16) | ((desc->base2) << 24);
}