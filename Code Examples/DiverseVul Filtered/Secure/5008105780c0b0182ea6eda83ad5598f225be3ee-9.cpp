static void ctypeof(JF, js_Ast *exp)
{
	if (exp->type == EXP_IDENTIFIER)
		emitlocal(J, F, OP_GETLOCAL, OP_HASVAR, exp);
	else
		cexp(J, F, exp);
	emit(J, F, OP_TYPEOF);
}