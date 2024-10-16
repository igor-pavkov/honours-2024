static int js_hasvar(js_State *J, const char *name)
{
	js_Environment *E = J->E;
	do {
		js_Property *ref = jsV_getproperty(J, E->variables, name);
		if (ref) {
			if (ref->getter) {
				js_pushobject(J, ref->getter);
				js_pushobject(J, E->variables);
				js_call(J, 0);
			} else {
				js_pushvalue(J, ref->value);
			}
			return 1;
		}
		E = E->outer;
	} while (E);
	return 0;
}