static js_Object *jsR_tofunction(js_State *J, int idx)
{
	js_Value *v = stackidx(J, idx);
	if (v->type == JS_TUNDEFINED || v->type == JS_TNULL)
		return NULL;
	if (v->type == JS_TOBJECT)
		if (v->u.object->type == JS_CFUNCTION || v->u.object->type == JS_CCFUNCTION)
			return v->u.object;
	js_typeerror(J, "not a function");
}