void test_delta_apply__read_after_limit(void)
{
	unsigned char base[16] = { 0 }, delta[] = { 0x10, 0x70, 0xff };
	void *out;
	size_t outlen;

	cl_git_fail(git_delta_apply(&out, &outlen, base, sizeof(base), delta, sizeof(delta)));
}