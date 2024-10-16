static void js_setvar(js_State *J, const char *name)
{
	js_Environment *E = J->E;
	do {
		js_Property *ref = jsV_getproperty(J, E->variables, name);
		if (ref) {
			if (ref->setter) {
				js_pushobject(J, ref->setter);
				js_pushobject(J, E->variables);
				js_copy(J, -3);
				js_call(J, 1);
				js_pop(J, 1);
				return;
			}
			if (!(ref->atts & JS_READONLY))
				ref->value = *stackidx(J, -1);
			else if (J->strict)
				js_typeerror(J, "'%s' is read-only", name);
			return;
		}
		E = E->outer;
	} while (E);
	if (J->strict)
		js_referenceerror(J, "assignment to undeclared variable '%s'", name);
	jsR_setproperty(J, J->G, name);
}