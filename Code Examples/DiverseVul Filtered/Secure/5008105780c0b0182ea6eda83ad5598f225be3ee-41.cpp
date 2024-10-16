static void ctryfinally(JF, js_Ast *trystm, js_Ast *finallystm)
{
	int L1;
	L1 = emitjump(J, F, OP_TRY);
	{
		/* if we get here, we have caught an exception in the try block */
		cstm(J, F, finallystm); /* inline finally block */
		emit(J, F, OP_THROW); /* rethrow exception */
	}
	label(J, F, L1);
	cstm(J, F, trystm);
	emit(J, F, OP_ENDTRY);
	cstm(J, F, finallystm);
}