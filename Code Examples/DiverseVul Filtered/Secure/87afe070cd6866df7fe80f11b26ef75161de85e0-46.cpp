static GF_Err do_compress_top_boxes(char *inName, char *outName)
{
	FILE *in, *out;
	u8 *buf;
	u32 buf_alloc, comp_size;
	s32 bytes_comp=0;
	s32 bytes_uncomp=0;
	GF_Err e = GF_OK;
	u64 source_size, dst_size;
	u32 orig_box_overhead;
	u32 final_box_overhead;
	u32 nb_added_box_bytes=0;
	Bool has_mov = GF_FALSE;
	Bool replace_all = !strcmp(compress_top_boxes, "*");
	GF_BitStream *bs_in, *bs_out;

	if (!outName) {
		M4_LOG(GF_LOG_ERROR, ("Missing output file name\n"));
		return GF_BAD_PARAM;
	}

	in = gf_fopen(inName, "rb");
	if (!in) return GF_URL_ERROR;
	out = gf_fopen(outName, "wb");
	if (!out) return GF_IO_ERR;

	buf_alloc = 4096;
	buf = gf_malloc(buf_alloc);

	bs_in = gf_bs_from_file(in, GF_BITSTREAM_READ);
	source_size = gf_bs_get_size(bs_in);

	bs_out = gf_bs_from_file(out, GF_BITSTREAM_WRITE);

	orig_box_overhead = 0;
	final_box_overhead = 0;
	while (gf_bs_available(bs_in)) {
		u32 size = gf_bs_read_u32(bs_in);
		u32 type = gf_bs_read_u32(bs_in);
		const char *b4cc = gf_4cc_to_str(type);
		const u8 *replace = (const u8 *) strstr(compress_top_boxes, b4cc);
		if (!strcmp(b4cc, "moov")) has_mov = GF_TRUE;

		if (!replace && !replace_all) {
			gf_bs_write_u32(bs_out, size);
			gf_bs_write_u32(bs_out, type);

			size-=8;
			while (size) {
				u32 nbytes = size;
				if (nbytes>buf_alloc) nbytes=buf_alloc;
				gf_bs_read_data(bs_in, buf, nbytes);
				gf_bs_write_data(bs_out, buf, nbytes);
				size-=nbytes;
			}
			continue;
		}
		orig_box_overhead += size;

		size-=8;

		if (size>buf_alloc) {
			buf_alloc = size;
			buf = gf_realloc(buf, buf_alloc);
		}
		gf_bs_read_data(bs_in, buf, size);

		replace+=5;

		comp_size = buf_alloc;

		e = gf_gz_compress_payload(&buf, size, &comp_size);
		if (e) break;

		if (comp_size>buf_alloc) {
			buf_alloc = comp_size;
		}
		bytes_uncomp += size;
		bytes_comp += comp_size;

		//write size
		gf_bs_write_u32(bs_out, comp_size+8);
		//write type
		gf_bs_write_data(bs_out, replace, 4);
		//write data
		gf_bs_write_data(bs_out, buf, comp_size);

		final_box_overhead += 8+comp_size;
	}
	dst_size = gf_bs_get_position(bs_out);

	if (buf) gf_free(buf);
	gf_bs_del(bs_in);
	gf_bs_del(bs_out);
	gf_fclose(in);
	gf_fclose(out);
	if (e) {
		M4_LOG(GF_LOG_ERROR, ("Error compressing: %s\n", gf_error_to_string(e)));
		return e;
	}

	if (has_mov) {
		u32 i, nb_tracks, nb_samples;
		GF_ISOFile *mov;
		Double rate, new_rate, duration;

		mov = gf_isom_open(inName, GF_ISOM_OPEN_READ, NULL);
		duration = (Double) gf_isom_get_duration(mov);
		duration /= gf_isom_get_timescale(mov);

		nb_samples = 0;
		nb_tracks = gf_isom_get_track_count(mov);
		for (i=0; i<nb_tracks; i++) {
			nb_samples += gf_isom_get_sample_count(mov, i+1);
		}
		gf_isom_close(mov);

		rate = (Double) source_size;
		rate /= duration;
		rate /= 1000;

		new_rate = (Double) dst_size;
		new_rate /= duration;
		new_rate /= 1000;


		fprintf(stderr, "Log format:\nname\torig\tcomp\tgain\tadded_bytes\torate\tcrate\tsamples\tduration\tobbps\tcbbps\n");
		fprintf(stdout, "%s\t%d\t%d\t%g\t%d\t%g\t%g\t%d\t%g\t%g\t%g\n", inName, bytes_uncomp, bytes_comp, ((Double)(bytes_uncomp-bytes_comp)*100)/bytes_uncomp, nb_added_box_bytes, rate, new_rate, nb_samples, duration, ((Double)orig_box_overhead)/nb_samples, ((Double)final_box_overhead)/nb_samples );

		fprintf(stderr, "%s Compressing top-level boxes saved %d bytes out of %d (reduced by %g %%) additional bytes %d original rate %g kbps new rate %g kbps, orig %g box bytes/sample final %g bytes/sample\n", inName, bytes_uncomp-bytes_comp, bytes_uncomp, ((Double)(bytes_uncomp-bytes_comp)*100)/bytes_uncomp, nb_added_box_bytes, rate, new_rate, ((Double)orig_box_overhead)/nb_samples, ((Double)final_box_overhead)/nb_samples );

	} else {
		fprintf(stderr, "Log format:\nname\torig\tcomp\tgain\tadded_bytes\n");
		fprintf(stdout, "%s\t%d\t%d\t%g\t%d\n", inName, bytes_uncomp, bytes_comp, ((Double) (bytes_uncomp - bytes_comp)*100)/(bytes_uncomp), nb_added_box_bytes);

		fprintf(stderr, "%s Compressing top-level boxes saved %d bytes out of %d (reduced by %g %%) additional bytes %d\n", inName, bytes_uncomp-bytes_comp, bytes_uncomp, ((Double)(bytes_uncomp-bytes_comp)*100)/bytes_uncomp, nb_added_box_bytes);

	}
	return GF_OK;
}