js_Regexp *js_toregexp(js_State *J, int idx)
{
	js_Value *v = stackidx(J, idx);
	if (v->type == JS_TOBJECT && v->u.object->type == JS_CREGEXP)
		return &v->u.object->u.r;
	js_typeerror(J, "not a regexp");
}