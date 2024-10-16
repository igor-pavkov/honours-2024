static js_Function *newfun(js_State *J, js_Ast *name, js_Ast *params, js_Ast *body, int script)
{
	js_Function *F = js_malloc(J, sizeof *F);
	memset(F, 0, sizeof *F);
	F->gcmark = 0;
	F->gcnext = J->gcfun;
	J->gcfun = F;
	++J->gccounter;

	F->filename = js_intern(J, J->filename);
	F->line = name ? name->line : params ? params->line : body ? body->line : 1;
	F->script = script;
	F->name = name ? name->string : "";

	cfunbody(J, F, name, params, body);

	return F;
}