static void cassignforin(JF, js_Ast *stm)
{
	js_Ast *lhs = stm->a;

	if (stm->type == STM_FOR_IN_VAR) {
		if (lhs->b)
			jsC_error(J, lhs->b, "more than one loop variable in for-in statement");
		emitlocal(J, F, OP_SETLOCAL, OP_SETVAR, lhs->a->a); /* list(var-init(ident)) */
		emit(J, F, OP_POP);
		return;
	}

	switch (lhs->type) {
	case EXP_IDENTIFIER:
		emitlocal(J, F, OP_SETLOCAL, OP_SETVAR, lhs);
		emit(J, F, OP_POP);
		break;
	case EXP_INDEX:
		cexp(J, F, lhs->a);
		cexp(J, F, lhs->b);
		emit(J, F, OP_ROT3);
		emit(J, F, OP_SETPROP);
		emit(J, F, OP_POP);
		break;
	case EXP_MEMBER:
		cexp(J, F, lhs->a);
		emit(J, F, OP_ROT2);
		emitstring(J, F, OP_SETPROP_S, lhs->b->string);
		emit(J, F, OP_POP);
		break;
	default:
		jsC_error(J, lhs, "invalid l-value in for-in loop assignment");
	}
}