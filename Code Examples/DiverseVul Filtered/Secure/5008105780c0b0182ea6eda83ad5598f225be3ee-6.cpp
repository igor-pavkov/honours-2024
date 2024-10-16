static void ceval(JF, js_Ast *fun, js_Ast *args)
{
	int n = cargs(J, F, args);
	if (n == 0)
		emit(J, F, OP_UNDEF);
	else while (n-- > 1)
		emit(J, F, OP_POP);
	emit(J, F, OP_EVAL);
}