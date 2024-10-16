static void cassignop(JF, js_Ast *exp, int opcode)
{
	js_Ast *lhs = exp->a;
	js_Ast *rhs = exp->b;
	cassignop1(J, F, lhs);
	cexp(J, F, rhs);
	emit(J, F, opcode);
	cassignop2(J, F, lhs, 0);
}