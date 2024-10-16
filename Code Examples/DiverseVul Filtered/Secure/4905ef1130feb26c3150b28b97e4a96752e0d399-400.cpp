merge_stacks (VerifyContext *ctx, ILCodeDesc *from, ILCodeDesc *to, gboolean start, gboolean external) 
{
	MonoError error;
	int i, j, k;
	stack_init (ctx, to);

	if (start) {
		if (to->flags == IL_CODE_FLAG_NOT_PROCESSED) 
			from->size = 0;
		else
			stack_copy (&ctx->eval, to);
		goto end_verify;
	} else if (!(to->flags & IL_CODE_STACK_MERGED)) {
		stack_copy (to, &ctx->eval);
		goto end_verify;
	}
	VERIFIER_DEBUG ( printf ("performing stack merge %d x %d\n", from->size, to->size); );

	if (from->size != to->size) {
		VERIFIER_DEBUG ( printf ("different stack sizes %d x %d at 0x%04x\n", from->size, to->size, ctx->ip_offset); );
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Could not merge stacks, different sizes (%d x %d) at 0x%04x", from->size, to->size, ctx->ip_offset)); 
		goto end_verify;
	}

	//FIXME we need to preserve CMMP attributes
	//FIXME we must take null literals into consideration.
	for (i = 0; i < from->size; ++i) {
		ILStackDesc *new_slot = from->stack + i;
		ILStackDesc *old_slot = to->stack + i;
		MonoType *new_type = mono_type_from_stack_slot (new_slot);
		MonoType *old_type = mono_type_from_stack_slot (old_slot);
		MonoClass *old_class = mono_class_from_mono_type (old_type);
		MonoClass *new_class = mono_class_from_mono_type (new_type);
		MonoClass *match_class = NULL;

		// S := T then U = S (new value is compatible with current value, keep current)
		if (verify_stack_type_compatibility (ctx, old_type, new_slot)) {
			copy_stack_value (new_slot, old_slot);
			continue;
		}

		// T := S then U = T (old value is compatible with current value, use new)
		if (verify_stack_type_compatibility (ctx, new_type, old_slot)) {
			copy_stack_value (old_slot, new_slot);
			continue;
		}

		if (mono_type_is_generic_argument (old_type) || mono_type_is_generic_argument (new_type)) {
			char *old_name = stack_slot_full_name (old_slot); 
			char *new_name = stack_slot_full_name (new_slot);
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Could not merge stack at depth %d, types not compatible: %s X %s at 0x%04x", i, old_name, new_name, ctx->ip_offset));
			g_free (old_name);
			g_free (new_name);
			goto end_verify;			
		} 

		//both are reference types, use closest common super type
		if (!mono_class_from_mono_type (old_type)->valuetype 
			&& !mono_class_from_mono_type (new_type)->valuetype
			&& !stack_slot_is_managed_pointer (old_slot)
			&& !stack_slot_is_managed_pointer (new_slot)) {
			
			for (j = MIN (old_class->idepth, new_class->idepth) - 1; j > 0; --j) {
				if (mono_metadata_type_equal (&old_class->supertypes [j]->byval_arg, &new_class->supertypes [j]->byval_arg)) {
					match_class = old_class->supertypes [j];
					goto match_found;
				}
			}

			mono_class_setup_interfaces (old_class, &error);
			if (!mono_error_ok (&error)) {
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot merge stacks due to a TypeLoadException %s at 0x%04x", mono_error_get_message (&error), ctx->ip_offset));
				mono_error_cleanup (&error);
				goto end_verify;
			}
			for (j = 0; j < old_class->interface_count; ++j) {
				for (k = 0; k < new_class->interface_count; ++k) {
					if (mono_metadata_type_equal (&old_class->interfaces [j]->byval_arg, &new_class->interfaces [k]->byval_arg)) {
						match_class = old_class->interfaces [j];
						goto match_found;
					}
				}
			}

			//No decent super type found, use object
			match_class = mono_defaults.object_class;
			goto match_found;
		} else if (is_compatible_boxed_valuetype (ctx,old_type, new_type, new_slot, FALSE) || is_compatible_boxed_valuetype (ctx, new_type, old_type, old_slot, FALSE)) {
			match_class = mono_defaults.object_class;
			goto match_found;
		}

		{
		char *old_name = stack_slot_full_name (old_slot); 
		char *new_name = stack_slot_full_name (new_slot);
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Could not merge stack at depth %d, types not compatible: %s X %s at 0x%04x", i, old_name, new_name, ctx->ip_offset)); 
		g_free (old_name);
		g_free (new_name);
		}
		set_stack_value (ctx, old_slot, &new_class->byval_arg, stack_slot_is_managed_pointer (old_slot));
		goto end_verify;

match_found:
		g_assert (match_class);
		set_stack_value (ctx, old_slot, &match_class->byval_arg, stack_slot_is_managed_pointer (old_slot));
		set_stack_value (ctx, new_slot, &match_class->byval_arg, stack_slot_is_managed_pointer (old_slot));
		continue;
	}

end_verify:
	if (external)
		to->flags |= IL_CODE_FLAG_WAS_TARGET;
	to->flags |= IL_CODE_STACK_MERGED;
}