void js_rot4(js_State *J)
{
	/* A B C D -> D A B C */
	js_Value tmp = STACK[TOP-1];	/* A B C D (D) */
	STACK[TOP-1] = STACK[TOP-2];	/* A B C C */
	STACK[TOP-2] = STACK[TOP-3];	/* A B B C */
	STACK[TOP-3] = STACK[TOP-4];	/* A A B C */
	STACK[TOP-4] = tmp;		/* D A B C */
}