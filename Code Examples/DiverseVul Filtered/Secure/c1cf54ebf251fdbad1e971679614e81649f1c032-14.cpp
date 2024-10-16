void lxc_free_array(void **array, lxc_free_fn element_free_fn)
{
	void **p;

	for (p = array; p && *p; p++)
		element_free_fn(*p);

	free((void*)array);
}