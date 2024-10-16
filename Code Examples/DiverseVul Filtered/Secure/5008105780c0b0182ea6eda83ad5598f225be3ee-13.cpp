static void cfundecs(JF, js_Ast *list)
{
	while (list) {
		js_Ast *stm = list->a;
		if (stm->type == AST_FUNDEC) {
			emitfunction(J, F, newfun(J, stm->a, stm->b, stm->c, 0));
			emitstring(J, F, OP_INITVAR, stm->a->string);
		}
		list = list->b;
	}
}