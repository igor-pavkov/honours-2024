js_String *jsV_newmemstring(js_State *J, const char *s, int n)
{
	js_String *v = js_malloc(J, soffsetof(js_String, p) + n + 1);
	memcpy(v->p, s, n);
	v->p[n] = 0;
	v->gcmark = 0;
	v->gcnext = J->gcstr;
	J->gcstr = v;
	++J->gccounter;
	return v;
}