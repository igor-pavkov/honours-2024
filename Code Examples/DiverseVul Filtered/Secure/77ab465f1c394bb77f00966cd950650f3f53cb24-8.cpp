void js_rot3pop2(js_State *J)
{
	/* A B C -> C */
	STACK[TOP-3] = STACK[TOP-1];
	TOP -= 2;
}