void js_pushobject(js_State *J, js_Object *v)
{
	CHECKSTACK(1);
	STACK[TOP].type = JS_TOBJECT;
	STACK[TOP].u.object = v;
	++TOP;
}