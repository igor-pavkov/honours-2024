void js_pushglobal(js_State *J)
{
	js_pushobject(J, J->G);
}