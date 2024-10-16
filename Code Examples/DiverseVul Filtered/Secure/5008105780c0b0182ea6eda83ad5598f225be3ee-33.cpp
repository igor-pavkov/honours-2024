static void cassign(JF, js_Ast *exp)
{
	js_Ast *lhs = exp->a;
	js_Ast *rhs = exp->b;
	switch (lhs->type) {
	case EXP_IDENTIFIER:
		cexp(J, F, rhs);
		emitlocal(J, F, OP_SETLOCAL, OP_SETVAR, lhs);
		break;
	case EXP_INDEX:
		cexp(J, F, lhs->a);
		cexp(J, F, lhs->b);
		cexp(J, F, rhs);
		emit(J, F, OP_SETPROP);
		break;
	case EXP_MEMBER:
		cexp(J, F, lhs->a);
		cexp(J, F, rhs);
		emitstring(J, F, OP_SETPROP_S, lhs->b->string);
		break;
	default:
		jsC_error(J, lhs, "invalid l-value in assignment");
	}
}