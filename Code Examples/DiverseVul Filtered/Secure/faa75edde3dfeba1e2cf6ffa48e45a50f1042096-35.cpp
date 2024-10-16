static void lsr_read_value_with_units(GF_LASeRCodec *lsr, SVG_Number *n, const char *name)
{
	s32 val;
	GF_LSR_READ_INT(lsr, val, 32, name);
#ifdef GPAC_FIXED_POINT
	n->value = val << 8;
#else
	n->value = INT2FIX(val) / (1<<8);
#endif
	GF_LSR_READ_INT(lsr, val, 3, "units");
	switch (val) {
	case 1:
		n->type = SVG_NUMBER_IN;
		break;
	case 2:
		n->type = SVG_NUMBER_CM;
		break;
	case 3:
		n->type = SVG_NUMBER_MM;
		break;
	case 4:
		n->type = SVG_NUMBER_PT;
		break;
	case 5:
		n->type = SVG_NUMBER_PC;
		break;
	case 6:
		n->type = SVG_NUMBER_PERCENTAGE;
		break;
	default:
		n->type = SVG_NUMBER_VALUE;
		break;
	}
}