int js_isregexp(js_State *J, int idx)
{
	js_Value *v = stackidx(J, idx);
	return v->type == JS_TOBJECT && v->u.object->type == JS_CREGEXP;
}