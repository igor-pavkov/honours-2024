static void lsr_read_point_sequence(GF_LASeRCodec *lsr, GF_List *pts, const char *name)
{
	u32 flag, i, count;

	while (gf_list_count(pts)) {
		SVG_Point *v = (SVG_Point *)gf_list_last(pts);
		gf_list_rem_last(pts);
		gf_free(v);
	}
	count = lsr_read_vluimsbf5(lsr, "nbPoints");
	if (!count) return;
	/*TODO golomb coding*/
	GF_LSR_READ_INT(lsr, flag, 1, "flag");
	if (!flag) {
		if (count < 3) {
			u32 nb_bits, v;
			GF_LSR_READ_INT(lsr, nb_bits, 5, "bits");
			for (i=0; i<count; i++) {
				SVG_Point *pt = (SVG_Point *)gf_malloc(sizeof(SVG_Point));
				if (!pt) {
					lsr->last_error = GF_OUT_OF_MEM;
				} else {
					gf_list_add(pts, pt);
					GF_LSR_READ_INT(lsr, v, nb_bits, "x");
					pt->x = lsr_translate_coords(lsr, v, nb_bits);
					GF_LSR_READ_INT(lsr, v, nb_bits, "y");
					pt->y = lsr_translate_coords(lsr, v, nb_bits);
				}
				if (lsr->last_error) return;
			}
		} else {
			u32 nb_dx, nb_dy, k;
			Fixed x, y;
			SVG_Point *pt = (SVG_Point *)gf_malloc(sizeof(SVG_Point));
			if (!pt) {
				lsr->last_error = GF_OUT_OF_MEM;
				return;
			}
			gf_list_add(pts, pt);

			GF_LSR_READ_INT(lsr, nb_dx, 5, "bits");
			GF_LSR_READ_INT(lsr, k, nb_dx, "x");
			x = pt->x = lsr_translate_coords(lsr, k, nb_dx);
			GF_LSR_READ_INT(lsr, k, nb_dx, "y");
			y = pt->y = lsr_translate_coords(lsr, k, nb_dx);

			GF_LSR_READ_INT(lsr, nb_dx, 5, "bitsx");
			GF_LSR_READ_INT(lsr, nb_dy, 5, "bitsy");
			for (i=1; i<count; i++) {
				pt = (SVG_Point *)gf_malloc(sizeof(SVG_Point));
				if (!pt) {
					lsr->last_error = GF_OUT_OF_MEM;
				} else {
					gf_list_add(pts, pt);
					GF_LSR_READ_INT(lsr, k, nb_dx, "dx");
					pt->x = x + lsr_translate_coords(lsr, k, nb_dx);
					x = pt->x;
					GF_LSR_READ_INT(lsr, k, nb_dy, "dy");
					pt->y = y + lsr_translate_coords(lsr, k, nb_dy);
					y = pt->y;
				}
				if (lsr->last_error) return;
			}
		}
	}
}