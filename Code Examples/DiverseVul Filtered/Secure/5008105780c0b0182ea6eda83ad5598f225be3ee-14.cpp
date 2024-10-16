static int emitjump(JF, int opcode)
{
	int inst = F->codelen + 1;
	emit(J, F, opcode);
	emitraw(J, F, 0);
	return inst;
}