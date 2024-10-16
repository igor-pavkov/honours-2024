static void emitraw(JF, int value)
{
	if (value != (js_Instruction)value)
		js_syntaxerror(J, "integer overflow in instruction coding");
	if (F->codelen >= F->codecap) {
		F->codecap = F->codecap ? F->codecap * 2 : 64;
		F->code = js_realloc(J, F->code, F->codecap * sizeof *F->code);
	}
	F->code[F->codelen++] = value;
}