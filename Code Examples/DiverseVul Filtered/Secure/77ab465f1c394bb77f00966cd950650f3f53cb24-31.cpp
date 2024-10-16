void *js_realloc(js_State *J, void *ptr, int size)
{
	ptr = J->alloc(J->actx, ptr, size);
	if (!ptr)
		js_outofmemory(J);
	return ptr;
}