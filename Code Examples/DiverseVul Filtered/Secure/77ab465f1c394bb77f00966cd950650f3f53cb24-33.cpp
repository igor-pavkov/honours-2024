int js_isarray(js_State *J, int idx)
{
	js_Value *v = stackidx(J, idx);
	return v->type == JS_TOBJECT && v->u.object->type == JS_CARRAY;
}