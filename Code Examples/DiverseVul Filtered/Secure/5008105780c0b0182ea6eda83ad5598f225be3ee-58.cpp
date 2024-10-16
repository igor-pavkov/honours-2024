static void emitjumpto(JF, int opcode, int dest)
{
	emit(J, F, opcode);
	if (dest != (js_Instruction)dest)
		js_syntaxerror(J, "jump address integer overflow");
	emitraw(J, F, dest);
}