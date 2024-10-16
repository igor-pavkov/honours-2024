static void emitfunction(JF, js_Function *fun)
{
	emit(J, F, OP_CLOSURE);
	emitraw(J, F, addfunction(J, F, fun));
}