void js_free(js_State *J, void *ptr)
{
	J->alloc(J->actx, ptr, 0);
}