static void jsR_defproperty(js_State *J, js_Object *obj, const char *name,
	int atts, js_Value *value, js_Object *getter, js_Object *setter)
{
	js_Property *ref;
	int k;

	if (obj->type == JS_CARRAY) {
		if (!strcmp(name, "length"))
			goto readonly;
	}

	else if (obj->type == JS_CSTRING) {
		if (!strcmp(name, "length"))
			goto readonly;
		if (js_isarrayindex(J, name, &k))
			if (js_runeat(J, obj->u.s.string, k))
				goto readonly;
	}

	else if (obj->type == JS_CREGEXP) {
		if (!strcmp(name, "source")) goto readonly;
		if (!strcmp(name, "global")) goto readonly;
		if (!strcmp(name, "ignoreCase")) goto readonly;
		if (!strcmp(name, "multiline")) goto readonly;
		if (!strcmp(name, "lastIndex")) goto readonly;
	}

	else if (obj->type == JS_CUSERDATA) {
		if (obj->u.user.put && obj->u.user.put(J, obj->u.user.data, name))
			return;
	}

	ref = jsV_setproperty(J, obj, name);
	if (ref) {
		if (value) {
			if (!(ref->atts & JS_READONLY))
				ref->value = *value;
			else if (J->strict)
				js_typeerror(J, "'%s' is read-only", name);
		}
		if (getter) {
			if (!(ref->atts & JS_DONTCONF))
				ref->getter = getter;
			else if (J->strict)
				js_typeerror(J, "'%s' is non-configurable", name);
		}
		if (setter) {
			if (!(ref->atts & JS_DONTCONF))
				ref->setter = setter;
			else if (J->strict)
				js_typeerror(J, "'%s' is non-configurable", name);
		}
		ref->atts |= atts;
	}

	return;

readonly:
	if (J->strict)
		js_typeerror(J, "'%s' is read-only or non-configurable", name);
}