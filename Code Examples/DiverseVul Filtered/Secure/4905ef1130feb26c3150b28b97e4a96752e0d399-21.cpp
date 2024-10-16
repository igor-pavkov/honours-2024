do_boolean_branch_op (VerifyContext *ctx, int delta)
{
	int target = ctx->ip_offset + delta;
	ILStackDesc *top;

	VERIFIER_DEBUG ( printf ("boolean branch offset %d delta %d target %d\n", ctx->ip_offset, delta, target); );
 
	if (target < 0 || target >= ctx->code_size) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Boolean branch target out of code at 0x%04x", ctx->ip_offset));
		return;
	}

	switch (is_valid_branch_instruction (ctx->header, ctx->ip_offset, target)) {
	case 1:
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Branch target escapes out of exception block at 0x%04x", ctx->ip_offset));
		break;
	case 2:
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Branch target escapes out of exception block at 0x%04x", ctx->ip_offset));
		return;
	}

	ctx->target = target;

	if (!check_underflow (ctx, 1))
		return;

	top = stack_pop (ctx);
	if (!is_valid_bool_arg (top))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Argument type %s not valid for brtrue/brfalse at 0x%04x", stack_slot_get_name (top), ctx->ip_offset));

	check_unmanaged_pointer (ctx, top);
}