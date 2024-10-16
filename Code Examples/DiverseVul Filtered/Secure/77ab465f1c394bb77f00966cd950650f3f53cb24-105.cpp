int js_isuserdata(js_State *J, int idx, const char *tag)
{
	js_Value *v = stackidx(J, idx);
	if (v->type == JS_TOBJECT && v->u.object->type == JS_CUSERDATA)
		return !strcmp(tag, v->u.object->u.user.tag);
	return 0;
}