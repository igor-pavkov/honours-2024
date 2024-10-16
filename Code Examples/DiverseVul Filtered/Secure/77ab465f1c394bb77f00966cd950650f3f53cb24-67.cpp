void js_pushundefinedthis(js_State *J)
{
	if (J->strict)
		js_pushundefined(J);
	else
		js_pushobject(J, J->G);
}