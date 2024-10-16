do_branch_op (VerifyContext *ctx, signed int delta, const unsigned char table [TYPE_MAX][TYPE_MAX])
{
	ILStackDesc *a, *b;
	int idxa, idxb;
	unsigned char res;
	int target = ctx->ip_offset + delta;

	VERIFIER_DEBUG ( printf ("branch offset %d delta %d target %d\n", ctx->ip_offset, delta, target); );
 
	if (target < 0 || target >= ctx->code_size) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Branch target out of code at 0x%04x", ctx->ip_offset));
		return;
	}

	switch (is_valid_cmp_branch_instruction (ctx->header, ctx->ip_offset, target)) {
	case 1: /*FIXME use constants and not magic numbers.*/
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Branch target escapes out of exception block at 0x%04x", ctx->ip_offset));
		break;
	case 2:
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Branch target escapes out of exception block at 0x%04x", ctx->ip_offset));
		return;
	}

	ctx->target = target;

	if (!check_underflow (ctx, 2))
		return;

	b = stack_pop (ctx);
	a = stack_pop (ctx);

	idxa = stack_slot_get_underlying_type (a);
	if (stack_slot_is_managed_pointer (a))
		idxa = TYPE_PTR;

	idxb = stack_slot_get_underlying_type (b);
	if (stack_slot_is_managed_pointer (b))
		idxb = TYPE_PTR;

	if (stack_slot_is_complex_type_not_reference_type (a) || stack_slot_is_complex_type_not_reference_type (b)) {
		res = TYPE_INV;
	} else {
		--idxa;
		--idxb;
		res = table [idxa][idxb];
	}

	VERIFIER_DEBUG ( printf ("branch res %d\n", res); );
	VERIFIER_DEBUG ( printf ("idxa %d idxb %d\n", idxa, idxb); );

	if (res == TYPE_INV) {
		CODE_NOT_VERIFIABLE (ctx,
			g_strdup_printf ("Compare and Branch instruction applyed to ill formed stack (%s x %s) at 0x%04x", stack_slot_get_name (a), stack_slot_get_name (b), ctx->ip_offset));
	} else if (res & NON_VERIFIABLE_RESULT) {
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Compare and Branch instruction is not verifiable (%s x %s) at 0x%04x", stack_slot_get_name (a), stack_slot_get_name (b), ctx->ip_offset)); 
 		res = res & ~NON_VERIFIABLE_RESULT;
 	}
}