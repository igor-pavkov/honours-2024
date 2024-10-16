static unsigned int amd_size_cache(struct cpuinfo_x86 *c, unsigned int size)
{
	/* AMD errata T13 (order #21922) */
	if ((c->x86 == 6)) {
		/* Duron Rev A0 */
		if (c->x86_model == 3 && c->x86_mask == 0)
			size = 64;
		/* Tbird rev A1/A2 */
		if (c->x86_model == 4 &&
			(c->x86_mask == 0 || c->x86_mask == 1))
			size = 256;
	}
	return size;
}