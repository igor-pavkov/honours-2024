static GF_Err lsr_decode_laser_unit(GF_LASeRCodec *lsr, GF_List *com_list)
{
	GF_Err e;
	Bool reset_encoding_context;
	u32 flag, i, count = 0, privateDataIdentifierIndexBits;

	lsr->last_error = GF_OK;

	/*
	 *	1 - laser unit header
	 */
	GF_LSR_READ_INT(lsr, reset_encoding_context, 1, "resetEncodingContext");
	GF_LSR_READ_INT(lsr, flag, 1, "opt_group");
	if (flag) lsr_read_extension(lsr, "ext");

	/*clean all tables*/
	if (reset_encoding_context) {
		lsr->nb_cols = 0;
		if (lsr->col_table) gf_free(lsr->col_table);
		lsr->col_table = NULL;
		while (gf_list_count(lsr->font_table)) {
			char *ft = (char *)gf_list_last(lsr->font_table);
			gf_free(ft);
			gf_list_rem_last(lsr->font_table);
		}
		lsr->privateData_id_index = lsr->privateTag_index = 0;
	}

	/*
	 *	2 - codecInitialisations
	 */

	/*
	 * 2.a - condecInitialization.color
	 */
	GF_LSR_READ_INT(lsr, flag, 1, "colorInitialisation");

	if (flag) {
		count = lsr_read_vluimsbf5(lsr, "count");
		if (count>gf_bs_available(lsr->bs)) return lsr->last_error = GF_NON_COMPLIANT_BITSTREAM;
		lsr->col_table = (LSRCol*)gf_realloc(lsr->col_table, sizeof(LSRCol)*(lsr->nb_cols+count));
		if (!lsr->col_table)
			return lsr->last_error = GF_OUT_OF_MEM;
		for (i=0; i<count; i++) {
			LSRCol c;
			GF_LSR_READ_INT(lsr, c.r, lsr->info->cfg.colorComponentBits, "red");
			GF_LSR_READ_INT(lsr, c.g, lsr->info->cfg.colorComponentBits, "green");
			GF_LSR_READ_INT(lsr, c.b, lsr->info->cfg.colorComponentBits, "blue");
			lsr->col_table[lsr->nb_cols+i] = c;
			if (lsr->last_error) return lsr->last_error;
		}
		lsr->nb_cols += count;
	}
	lsr->colorIndexBits = gf_get_bit_size(lsr->nb_cols);
	/*
	 * 2.b - condecInitialization.fonts
	 */
	GF_LSR_READ_INT(lsr, flag, 1, "fontInitialisation");
	count = 0;
	if (flag) {
		count = lsr_read_vluimsbf5(lsr, "count");
		for (i=0; i<count; i++) {
			char *ft = NULL;
			lsr_read_byte_align_string(lsr, &ft, "font");
			gf_list_add(lsr->font_table, ft);
			if (lsr->last_error) return lsr->last_error;
		}
	}
	lsr->fontIndexBits = gf_get_bit_size(count);
	/*
	 * 2.c - condecInitialization.private
	 */
	GF_LSR_READ_INT(lsr, flag, 1, "privateDataIdentifierInitialisation");

	if (flag) {
		count = lsr_read_vluimsbf5(lsr, "nbPrivateDataIdentifiers");
		for (i=0; i<count; i++) {
			lsr->privateData_id_index++;
			lsr_read_byte_align_string(lsr, NULL, "privateDataIdentifier");
			if (lsr->last_error) return lsr->last_error;
		}
	}
	/*
	 * 2.d - condecInitialization.anyXML
	 */
	GF_LSR_READ_INT(lsr, flag, 1, "anyXMLInitialisation");

	if (flag) {
		privateDataIdentifierIndexBits = gf_get_bit_size(lsr->privateData_id_index);
		count = lsr_read_vluimsbf5(lsr, "nbTags");
		for (i=0; i<count; i++) {
			lsr->privateTag_index++;
			if (i) {
				/* uint(privateDataIdentifierIndexBits) = */
				GF_LSR_READ_INT(lsr, flag, privateDataIdentifierIndexBits, "privateDataIdentifierIndex");
				lsr_read_byte_align_string(lsr, NULL, "tag");
			}
			GF_LSR_READ_INT(lsr, flag, 1, "hasAttrs");
			if (flag) {
				u32 k, c2 = lsr_read_vluimsbf5(lsr, "nbAttrNames");
				for (k=0; k<c2; k++) {
					if (!i) {
						/* uint(privateDataIdentifierIndexBits) = */
						GF_LSR_READ_INT(lsr, flag, privateDataIdentifierIndexBits, "privateDataIdentifierIndex");
					}
					lsr_read_byte_align_string(lsr, NULL, "tag");
				}
			}
			if (lsr->last_error) return lsr->last_error;
		}
	}
	/*
	 * 2.e - condecInitialization.extension
	 */
	count = lsr_read_vluimsbf5(lsr, "countG");
	for (i=0; i<count; i++) {
		/*u32 locID = */lsr_read_vluimsbf5(lsr, "binaryIdForThisStringID");
		lsr_read_byte_align_string(lsr, NULL, "stringID");
		if (lsr->last_error) return lsr->last_error;
	}
	GF_LSR_READ_INT(lsr, flag, 1, "hasExtension");
	if (flag) {
		u32 len = lsr_read_vluimsbf5(lsr, "len");
		u32 pos = gf_bs_get_bit_offset(lsr->bs);

		count = lsr_read_vluimsbf5(lsr, "len");
		for (i=0; i<count; i++) {
			/*u32 locID = */lsr_read_vluimsbf5(lsr, "localStreamIdForThisGlobal");
			lsr_read_byte_align_string(lsr, NULL, "globalName");
			if (lsr->last_error) return lsr->last_error;
		}
		pos = gf_bs_get_bit_offset(lsr->bs) - pos;
		if (len<pos)
			return GF_NON_COMPLIANT_BITSTREAM;

		GF_LSR_READ_INT(lsr, flag, pos, "remainingData");
	}

	e = lsr_read_command_list(lsr, com_list, NULL, 1);
	GF_LSR_READ_INT(lsr, flag, 1, "opt_group");
	if (flag) lsr_read_extension(lsr, "ext");
	return e;
}