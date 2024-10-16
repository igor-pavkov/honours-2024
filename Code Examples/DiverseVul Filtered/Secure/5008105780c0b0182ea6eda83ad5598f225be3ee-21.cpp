static void emitnumber(JF, double num)
{
	if (num == 0) {
		emit(J, F, OP_NUMBER_0);
		if (signbit(num))
			emit(J, F, OP_NEG);
	} else if (num == 1) {
		emit(J, F, OP_NUMBER_1);
	} else if (num == (js_Instruction)num) {
		emit(J, F, OP_NUMBER_POS);
		emitraw(J, F, (js_Instruction)num);
	} else if (num < 0 && -num == (js_Instruction)(-num)) {
		emit(J, F, OP_NUMBER_NEG);
		emitraw(J, F, (js_Instruction)(-num));
	} else {
		emit(J, F, OP_NUMBER);
		emitraw(J, F, addnumber(J, F, num));
	}
}