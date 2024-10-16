static int jsR_delproperty(js_State *J, js_Object *obj, const char *name)
{
	js_Property *ref;
	int k;

	if (obj->type == JS_CARRAY) {
		if (!strcmp(name, "length"))
			goto dontconf;
	}

	else if (obj->type == JS_CSTRING) {
		if (!strcmp(name, "length"))
			goto dontconf;
		if (js_isarrayindex(J, name, &k))
			if (js_runeat(J, obj->u.s.string, k))
				goto dontconf;
	}

	else if (obj->type == JS_CREGEXP) {
		if (!strcmp(name, "source")) goto dontconf;
		if (!strcmp(name, "global")) goto dontconf;
		if (!strcmp(name, "ignoreCase")) goto dontconf;
		if (!strcmp(name, "multiline")) goto dontconf;
		if (!strcmp(name, "lastIndex")) goto dontconf;
	}

	else if (obj->type == JS_CUSERDATA) {
		if (obj->u.user.delete && obj->u.user.delete(J, obj->u.user.data, name))
			return 1;
	}

	ref = jsV_getownproperty(J, obj, name);
	if (ref) {
		if (ref->atts & JS_DONTCONF)
			goto dontconf;
		jsV_delproperty(J, obj, name);
	}
	return 1;

dontconf:
	if (J->strict)
		js_typeerror(J, "'%s' is non-configurable", name);
	return 0;
}