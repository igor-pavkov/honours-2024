mono_delegate_type_equal (MonoType *target, MonoType *candidate)
{
	if (candidate->byref ^ target->byref)
		return FALSE;

	switch (target->type) {
	case MONO_TYPE_VOID:
	case MONO_TYPE_I1:
	case MONO_TYPE_U1:
	case MONO_TYPE_BOOLEAN:
	case MONO_TYPE_I2:
	case MONO_TYPE_U2:
	case MONO_TYPE_CHAR:
	case MONO_TYPE_I4:
	case MONO_TYPE_U4:
	case MONO_TYPE_I8:
	case MONO_TYPE_U8:
	case MONO_TYPE_R4:
	case MONO_TYPE_R8:
	case MONO_TYPE_I:
	case MONO_TYPE_U:
	case MONO_TYPE_STRING:
	case MONO_TYPE_TYPEDBYREF:
		return candidate->type == target->type;

	case MONO_TYPE_PTR:
		return mono_delegate_type_equal (target->data.type, candidate->data.type);

	case MONO_TYPE_FNPTR:
		if (candidate->type != MONO_TYPE_FNPTR)
			return FALSE;
		return mono_delegate_signature_equal (mono_type_get_signature (target), mono_type_get_signature (candidate), FALSE);

	case MONO_TYPE_GENERICINST: {
		MonoClass *target_klass;
		MonoClass *candidate_klass;
		target_klass = mono_class_from_mono_type (target);
		candidate_klass = mono_class_from_mono_type (candidate);
		/*FIXME handle nullables and enum*/
		return mono_class_is_assignable_from (target_klass, candidate_klass);
	}
	case MONO_TYPE_OBJECT:
		return MONO_TYPE_IS_REFERENCE (candidate);

	case MONO_TYPE_CLASS:
		return mono_class_is_assignable_from(target->data.klass, mono_class_from_mono_type (candidate));

	case MONO_TYPE_SZARRAY:
		if (candidate->type != MONO_TYPE_SZARRAY)
			return FALSE;
		return mono_class_is_assignable_from (mono_class_from_mono_type (target)->element_class, mono_class_from_mono_type (candidate)->element_class);

	case MONO_TYPE_ARRAY:
		if (candidate->type != MONO_TYPE_ARRAY)
			return FALSE;
		return is_array_type_compatible (target, candidate);

	case MONO_TYPE_VALUETYPE:
		/*FIXME handle nullables and enum*/
		return mono_class_from_mono_type (candidate) == mono_class_from_mono_type (target);

	case MONO_TYPE_VAR:
		return candidate->type == MONO_TYPE_VAR && mono_type_get_generic_param_num (target) == mono_type_get_generic_param_num (candidate);
		return FALSE;

	case MONO_TYPE_MVAR:
		return candidate->type == MONO_TYPE_MVAR && mono_type_get_generic_param_num (target) == mono_type_get_generic_param_num (candidate);
		return FALSE;

	default:
		VERIFIER_DEBUG ( printf ("Unknown type %d. Implement me!\n", target->type); );
		g_assert_not_reached ();
		return FALSE;
	}
}