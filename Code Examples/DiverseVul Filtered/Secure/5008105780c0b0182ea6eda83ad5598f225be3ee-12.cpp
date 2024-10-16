js_Function *jsC_compile(js_State *J, js_Ast *prog)
{
	return newfun(J, NULL, NULL, prog, 1);
}