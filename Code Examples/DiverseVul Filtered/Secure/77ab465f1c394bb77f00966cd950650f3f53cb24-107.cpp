static void js_defvar(js_State *J, const char *name)
{
	jsR_defproperty(J, J->E->variables, name, JS_DONTENUM | JS_DONTCONF, NULL, NULL, NULL);
}