static int shadow_copy2_label_cmp_desc(const void *x, const void *y)
{
	return -strncmp((const char *)x, (const char *)y, sizeof(SHADOW_COPY_LABEL));
}