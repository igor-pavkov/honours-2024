void js_eval(js_State *J)
{
	if (!js_isstring(J, -1))
		return;
	js_loadeval(J, "(eval)", js_tostring(J, -1));
	js_rot2pop1(J);
	js_copy(J, 0); /* copy 'this' */
	js_call(J, 0);
}