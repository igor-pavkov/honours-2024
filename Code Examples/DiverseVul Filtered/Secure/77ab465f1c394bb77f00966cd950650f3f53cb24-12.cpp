static void jsR_setproperty(js_State *J, js_Object *obj, const char *name)
{
	js_Value *value = stackidx(J, -1);
	js_Property *ref;
	int k;
	int own;

	if (obj->type == JS_CARRAY) {
		if (!strcmp(name, "length")) {
			double rawlen = jsV_tonumber(J, value);
			int newlen = jsV_numbertointeger(rawlen);
			if (newlen != rawlen)
				js_rangeerror(J, "array length");
			jsV_resizearray(J, obj, newlen);
			return;
		}
		if (js_isarrayindex(J, name, &k))
			if (k >= obj->u.a.length)
				obj->u.a.length = k + 1;
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
		if (!strcmp(name, "lastIndex")) {
			obj->u.r.last = jsV_tointeger(J, value);
			return;
		}
	}

	else if (obj->type == JS_CUSERDATA) {
		if (obj->u.user.put && obj->u.user.put(J, obj->u.user.data, name))
			return;
	}

	/* First try to find a setter in prototype chain */
	ref = jsV_getpropertyx(J, obj, name, &own);
	if (ref && ref->setter) {
		js_pushobject(J, ref->setter);
		js_pushobject(J, obj);
		js_pushvalue(J, *value);
		js_call(J, 1);
		js_pop(J, 1);
		return;
	}

	/* Property not found on this object, so create one */
	if (!ref || !own)
		ref = jsV_setproperty(J, obj, name);

	if (ref) {
		if (!(ref->atts & JS_READONLY))
			ref->value = *value;
		else
			goto readonly;
	}

	return;

readonly:
	if (J->strict)
		js_typeerror(J, "'%s' is read-only", name);
}