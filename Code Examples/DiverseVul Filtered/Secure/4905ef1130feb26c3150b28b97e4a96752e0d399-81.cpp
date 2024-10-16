do_binop (VerifyContext *ctx, unsigned int opcode, const unsigned char table [TYPE_MAX][TYPE_MAX])
{
	ILStackDesc *a, *b, *top;
	int idxa, idxb, complexMerge = 0;
	unsigned char res;

	if (!check_underflow (ctx, 2))
		return;
	b = stack_pop (ctx);
	a = stack_pop (ctx);

	idxa = stack_slot_get_underlying_type (a);
	if (stack_slot_is_managed_pointer (a)) {
		idxa = TYPE_PTR;
		complexMerge = 1;
	}

	idxb = stack_slot_get_underlying_type (b);
	if (stack_slot_is_managed_pointer (b)) {
		idxb = TYPE_PTR;
		complexMerge = 2;
	}

	--idxa;
	--idxb;
	res = table [idxa][idxb];

	VERIFIER_DEBUG ( printf ("binop res %d\n", res); );
	VERIFIER_DEBUG ( printf ("idxa %d idxb %d\n", idxa, idxb); );

	top = stack_push (ctx);
	if (res == TYPE_INV) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Binary instruction applyed to ill formed stack (%s x %s)", stack_slot_get_name (a), stack_slot_get_name (b)));
		copy_stack_value (top, a);
		return;
	}

 	if (res & NON_VERIFIABLE_RESULT) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Binary instruction is not verifiable (%s x %s)", stack_slot_get_name (a), stack_slot_get_name (b)));

		res = res & ~NON_VERIFIABLE_RESULT;
 	}

 	if (complexMerge && res == TYPE_PTR) {
 		if (complexMerge == 1) 
 			copy_stack_value (top, a);
 		else if (complexMerge == 2)
 			copy_stack_value (top, b);
		/*
		 * There is no need to merge the type of two pointers.
		 * The only valid operation is subtraction, that returns a native
		 *  int as result and can be used with any 2 pointer kinds.
		 * This is valid acording to Patition III 1.1.4
		 */
 	} else
 		top->stype = res;
 	
}