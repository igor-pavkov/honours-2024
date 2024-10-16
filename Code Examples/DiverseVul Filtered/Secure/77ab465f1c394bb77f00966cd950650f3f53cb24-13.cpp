void js_rot2pop1(js_State *J)
{
	/* A B -> B */
	STACK[TOP-2] = STACK[TOP-1];
	--TOP;
}