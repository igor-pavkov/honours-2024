int js_iscallable(js_State *J, int idx)
{
	js_Value *v = stackidx(J, idx);
	if (v->type == JS_TOBJECT)
		return v->u.object->type == JS_CFUNCTION ||
			v->u.object->type == JS_CSCRIPT ||
			v->u.object->type == JS_CCFUNCTION;
	return 0;
}