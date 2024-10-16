static filter_pred_fn_t select_comparison_fn(enum filter_op_ids op,
					    int field_size, int field_is_signed)
{
	filter_pred_fn_t fn = NULL;
	int pred_func_index = -1;

	switch (op) {
	case OP_EQ:
	case OP_NE:
		break;
	default:
		if (WARN_ON_ONCE(op < PRED_FUNC_START))
			return NULL;
		pred_func_index = op - PRED_FUNC_START;
		if (WARN_ON_ONCE(pred_func_index > PRED_FUNC_MAX))
			return NULL;
	}

	switch (field_size) {
	case 8:
		if (pred_func_index < 0)
			fn = filter_pred_64;
		else if (field_is_signed)
			fn = pred_funcs_s64[pred_func_index];
		else
			fn = pred_funcs_u64[pred_func_index];
		break;
	case 4:
		if (pred_func_index < 0)
			fn = filter_pred_32;
		else if (field_is_signed)
			fn = pred_funcs_s32[pred_func_index];
		else
			fn = pred_funcs_u32[pred_func_index];
		break;
	case 2:
		if (pred_func_index < 0)
			fn = filter_pred_16;
		else if (field_is_signed)
			fn = pred_funcs_s16[pred_func_index];
		else
			fn = pred_funcs_u16[pred_func_index];
		break;
	case 1:
		if (pred_func_index < 0)
			fn = filter_pred_8;
		else if (field_is_signed)
			fn = pred_funcs_s8[pred_func_index];
		else
			fn = pred_funcs_u8[pred_func_index];
		break;
	}

	return fn;
}