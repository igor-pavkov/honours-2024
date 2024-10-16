void *js_malloc(js_State *J, int size)
{
	void *ptr = J->alloc(J->actx, NULL, size);
	if (!ptr)
		js_outofmemory(J);
	return ptr;
}