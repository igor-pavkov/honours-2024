do_newobj (VerifyContext *ctx, int token) 
{
	ILStackDesc *value;
	int i;
	MonoMethodSignature *sig;
	MonoMethod *method;
	gboolean is_delegate = FALSE;

	if (!(method = verifier_load_method (ctx, token, "newobj")))
		return;

	if (!mono_method_is_constructor (method)) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Method from token 0x%08x not a constructor at 0x%04x", token, ctx->ip_offset));
		return;
	}

	if (method->klass->flags & (TYPE_ATTRIBUTE_ABSTRACT | TYPE_ATTRIBUTE_INTERFACE))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Trying to instantiate an abstract or interface type at 0x%04x", ctx->ip_offset));

	if (!mono_method_can_access_method_full (ctx->method, method, NULL)) {
		char *from = mono_method_full_name (ctx->method, TRUE);
		char *to = mono_method_full_name (method, TRUE);
		CODE_NOT_VERIFIABLE2 (ctx, g_strdup_printf ("Constructor %s not visible from %s at 0x%04x", to, from, ctx->ip_offset), MONO_EXCEPTION_METHOD_ACCESS);
		g_free (from);
		g_free (to);
	}

	//FIXME use mono_method_get_signature_full
	sig = mono_method_signature (method);
	if (!sig) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid constructor signature to newobj at 0x%04x", ctx->ip_offset));
		return;
	}

	if (!check_underflow (ctx, sig->param_count))
		return;

	is_delegate = method->klass->parent == mono_defaults.multicastdelegate_class;

	if (is_delegate) {
		ILStackDesc *funptr;
		//first arg is object, second arg is fun ptr
		if (sig->param_count != 2) {
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid delegate constructor at 0x%04x", ctx->ip_offset));
			return;
		}
		funptr = stack_pop (ctx);
		value = stack_pop (ctx);
		verify_delegate_compatibility (ctx, method->klass, value, funptr);
	} else {
		for (i = sig->param_count - 1; i >= 0; --i) {
			VERIFIER_DEBUG ( printf ("verifying constructor argument %d\n", i); );
			value = stack_pop (ctx);
			if (!verify_stack_type_compatibility (ctx, sig->params [i], value)) {
				char *stack_name = stack_slot_full_name (value);
				char *sig_name = mono_type_full_name (sig->params [i]);
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Incompatible parameter value with constructor signature: %s X %s at 0x%04x", sig_name, stack_name, ctx->ip_offset));
				g_free (stack_name);
				g_free (sig_name);
			}

			if (stack_slot_is_managed_mutability_pointer (value))
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use a readonly pointer as argument of newobj at 0x%04x", ctx->ip_offset));
		}
	}

	if (check_overflow (ctx))
		set_stack_value (ctx, stack_push (ctx),  &method->klass->byval_arg, FALSE);
}