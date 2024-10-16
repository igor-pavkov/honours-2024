static void cdelete(JF, js_Ast *exp)
{
	switch (exp->type) {
	case EXP_IDENTIFIER:
		if (J->strict)
			jsC_error(J, exp, "delete on an unqualified name is not allowed in strict mode");
		emitlocal(J, F, OP_DELLOCAL, OP_DELVAR, exp);
		break;
	case EXP_INDEX:
		cexp(J, F, exp->a);
		cexp(J, F, exp->b);
		emit(J, F, OP_DELPROP);
		break;
	case EXP_MEMBER:
		cexp(J, F, exp->a);
		emitstring(J, F, OP_DELPROP_S, exp->b->string);
		break;
	default:
		jsC_error(J, exp, "invalid l-value in delete expression");
	}
}