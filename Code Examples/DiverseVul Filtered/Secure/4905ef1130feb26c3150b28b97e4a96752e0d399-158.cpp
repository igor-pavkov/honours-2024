do_ret (VerifyContext *ctx)
{
	MonoType *ret = ctx->signature->ret;
	VERIFIER_DEBUG ( printf ("checking ret\n"); );
	if (ret->type != MONO_TYPE_VOID) {
		ILStackDesc *top;
		if (!check_underflow (ctx, 1))
			return;

		top = stack_pop(ctx);

		if (!verify_stack_type_compatibility (ctx, ctx->signature->ret, top)) {
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Incompatible return value on stack with method signature ret at 0x%04x", ctx->ip_offset));
			return;
		}

		if (ret->byref || ret->type == MONO_TYPE_TYPEDBYREF || mono_type_is_value_type (ret, "System", "ArgIterator") || mono_type_is_value_type (ret, "System", "RuntimeArgumentHandle"))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Method returns byref, TypedReference, ArgIterator or RuntimeArgumentHandle at 0x%04x", ctx->ip_offset));
	}

	if (ctx->eval.size > 0) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Stack not empty (%d) after ret at 0x%04x", ctx->eval.size, ctx->ip_offset));
	} 
	if (in_any_block (ctx->header, ctx->ip_offset))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("ret cannot escape exception blocks at 0x%04x", ctx->ip_offset));
}