void js_pushiterator(js_State *J, int idx, int own)
{
	js_pushobject(J, jsV_newiterator(J, js_toobject(J, idx), own));
}