static test_param_t *get_test_param(void)
{
	static test_param_t test = { false };

	return &test;
}