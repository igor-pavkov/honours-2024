static int shadow_copy2_label_cmp_asc(const void *x, const void *y)
{
	return strncmp((const char *)x, (const char *)y, sizeof(SHADOW_COPY_LABEL));
}