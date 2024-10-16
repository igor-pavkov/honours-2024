static void labelto(JF, int inst, int addr)
{
	if (addr != (js_Instruction)addr)
		js_syntaxerror(J, "jump address integer overflow");
	F->code[inst] = addr;
}