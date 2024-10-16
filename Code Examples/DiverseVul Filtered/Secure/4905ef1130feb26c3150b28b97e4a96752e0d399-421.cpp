set_stack_value (VerifyContext *ctx, ILStackDesc *stack, MonoType *type, int take_addr)
{
	int mask = 0;
	int type_kind = type->type;

	if (type->byref || take_addr)
		mask = POINTER_MASK;
	/* TODO handle CMMP_MASK */

handle_enum:
	stack->type = type;

	switch (type_kind) {
	case MONO_TYPE_I1:
	case MONO_TYPE_U1:
	case MONO_TYPE_BOOLEAN:
	case MONO_TYPE_I2:
	case MONO_TYPE_U2:
	case MONO_TYPE_CHAR:
	case MONO_TYPE_I4:
	case MONO_TYPE_U4:
		stack->stype = TYPE_I4 | mask;
		break;
	case MONO_TYPE_I:
	case MONO_TYPE_U:
		stack->stype = TYPE_NATIVE_INT | mask;
		break;

	/*FIXME: Do we need to check if it's a pointer to the method pointer? The spec says it' illegal to have that.*/
	case MONO_TYPE_FNPTR:
	case MONO_TYPE_PTR:
	case MONO_TYPE_TYPEDBYREF:
		stack->stype = TYPE_PTR | mask;
		break;

	case MONO_TYPE_CLASS:
	case MONO_TYPE_STRING:
	case MONO_TYPE_OBJECT:
	case MONO_TYPE_SZARRAY:
	case MONO_TYPE_ARRAY:

	case MONO_TYPE_VAR:
	case MONO_TYPE_MVAR: 
		stack->stype = TYPE_COMPLEX | mask;
		break;
		
	case MONO_TYPE_I8:
	case MONO_TYPE_U8:
		stack->stype = TYPE_I8 | mask;
		break;
	case MONO_TYPE_R4:
	case MONO_TYPE_R8:
		stack->stype = TYPE_R8 | mask;
		break;
	case MONO_TYPE_GENERICINST:
	case MONO_TYPE_VALUETYPE:
		if (mono_type_is_enum_type (type)) {
			MonoType *utype = mono_type_get_underlying_type_any (type);
			if (!utype) {
				ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Could not resolve underlying type of %x at %d", type->type, ctx->ip_offset));
				return FALSE;
			}
			type = utype;
			type_kind = type->type;
			goto handle_enum;
		} else {
			stack->stype = TYPE_COMPLEX | mask;
			break;
		}
	default:
		VERIFIER_DEBUG ( printf ("unknown type 0x%02x in eval stack type\n", type->type); );
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Illegal value set on stack 0x%02x at %d", type->type, ctx->ip_offset));
		return FALSE;
	}
	return TRUE;
}