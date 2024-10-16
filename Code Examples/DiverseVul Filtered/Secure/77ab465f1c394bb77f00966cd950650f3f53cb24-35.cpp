int js_pconstruct(js_State *J, int n)
{
	int savetop = TOP - n - 2;
	if (js_try(J)) {
		/* clean up the stack to only hold the error object */
		STACK[savetop] = STACK[TOP-1];
		TOP = savetop + 1;
		return 1;
	}
	js_construct(J, n);
	js_endtry(J);
	return 0;
}