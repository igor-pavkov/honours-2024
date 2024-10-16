static void lsr_read_matrix(GF_LASeRCodec *lsr, SVG_Transform *mx)
{
	u32 flag;
	gf_mx2d_init(mx->mat);
	mx->is_ref = 0;
	GF_LSR_READ_INT(lsr, flag, 1, "isNotMatrix");
	if (flag) {
		GF_LSR_READ_INT(lsr, flag, 1, "isRef");
		if (flag) {
			GF_LSR_READ_INT(lsr, flag, 1, "hasXY");
			if (flag) {
				mx->mat.m[2] = lsr_read_fixed_16_8(lsr, "valueX");
				mx->mat.m[5] = lsr_read_fixed_16_8(lsr, "valueY");
			}
		} else {
			lsr_read_extension(lsr, "ext");
		}
	} else {
		lsr->coord_bits += lsr->scale_bits;
		GF_LSR_READ_INT(lsr, flag, 1, "xx_yy_present");
		if (flag) {
			GF_LSR_READ_INT(lsr, flag, lsr->coord_bits, "xx");
			mx->mat.m[0] = lsr_translate_scale(lsr, flag);
			GF_LSR_READ_INT(lsr, flag, lsr->coord_bits, "yy");
			mx->mat.m[4] = lsr_translate_scale(lsr, flag);
		} else {
			mx->mat.m[0] = mx->mat.m[4] = FIX_ONE;
		}
		GF_LSR_READ_INT(lsr, flag, 1, "xy_yx_present");
		if (flag) {
			GF_LSR_READ_INT(lsr, flag, lsr->coord_bits, "xy");
			mx->mat.m[1] = lsr_translate_scale(lsr, flag);
			GF_LSR_READ_INT(lsr, flag, lsr->coord_bits, "yx");
			mx->mat.m[3] = lsr_translate_scale(lsr, flag);
		}

		GF_LSR_READ_INT(lsr, flag, 1, "xz_yz_present");
		if (flag) {
			GF_LSR_READ_INT(lsr, flag, lsr->coord_bits, "xz");
			mx->mat.m[2] = lsr_translate_coords(lsr, flag, lsr->coord_bits);
			GF_LSR_READ_INT(lsr, flag, lsr->coord_bits, "yz");
			mx->mat.m[5] = lsr_translate_coords(lsr, flag, lsr->coord_bits);
		}
		lsr->coord_bits -= lsr->scale_bits;
	}
}