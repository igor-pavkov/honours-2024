js_Function *jsC_compilefunction(js_State *J, js_Ast *prog)
{
	return newfun(J, prog->a, prog->b, prog->c, 0);
}