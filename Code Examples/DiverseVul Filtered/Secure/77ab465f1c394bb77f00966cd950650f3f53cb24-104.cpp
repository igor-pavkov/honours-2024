const char *js_ref(js_State *J)
{
	js_Value *v = stackidx(J, -1);
	const char *s;
	char buf[32];
	switch (v->type) {
	case JS_TUNDEFINED: s = "_Undefined"; break;
	case JS_TNULL: s = "_Null"; break;
	case JS_TBOOLEAN:
		s = v->u.boolean ? "_True" : "_False";
		break;
	case JS_TOBJECT:
		sprintf(buf, "%p", (void*)v->u.object);
		s = js_intern(J, buf);
		break;
	default:
		sprintf(buf, "%d", J->nextref++);
		s = js_intern(J, buf);
		break;
	}
	js_setregistry(J, s);
	return s;
}