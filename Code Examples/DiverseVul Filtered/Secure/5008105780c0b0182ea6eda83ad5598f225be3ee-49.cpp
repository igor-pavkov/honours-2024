static void ccall(JF, js_Ast *fun, js_Ast *args)
{
	int n;
	switch (fun->type) {
	case EXP_INDEX:
		cexp(J, F, fun->a);
		emit(J, F, OP_DUP);
		cexp(J, F, fun->b);
		emit(J, F, OP_GETPROP);
		emit(J, F, OP_ROT2);
		break;
	case EXP_MEMBER:
		cexp(J, F, fun->a);
		emit(J, F, OP_DUP);
		emitstring(J, F, OP_GETPROP_S, fun->b->string);
		emit(J, F, OP_ROT2);
		break;
	case EXP_IDENTIFIER:
		if (!strcmp(fun->string, "eval")) {
			ceval(J, F, fun, args);
			return;
		}
		/* fall through */
	default:
		cexp(J, F, fun);
		emit(J, F, J->strict ? OP_UNDEF : OP_GLOBAL);
		break;
	}
	n = cargs(J, F, args);
	emit(J, F, OP_CALL);
	emitraw(J, F, n);
}