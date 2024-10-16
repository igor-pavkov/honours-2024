static u32 vvc_get_num_entry_points(VVCSliceInfo *si, u32 sh_slice_address, u32 sh_num_tiles_in_slice)
{
	if (!si->sps->entry_point_offsets_present_flag) return 0;

	u32 nb_entry_points = 0;
	u32 prev_ctb_addr_y=0;
	u32 prev_ctb_to_tile_row_bd, prev_ctb_to_tile_col_bd;
	s32 i;
	s32 nb_ctus_in_slice = vvc_get_ctb_info_in_slice(si, sh_slice_address, sh_num_tiles_in_slice, -1);
	if (nb_ctus_in_slice<0) return 0;

	for (i=0; i < nb_ctus_in_slice; i++ ) {
		s32 addr;
		u32 ctb_addr_x, ctb_addr_y;
		u32 ctb_to_tile_row_bd, ctb_to_tile_col_bd;

		addr = vvc_get_ctb_info_in_slice(si, sh_slice_address, sh_num_tiles_in_slice, i);
		if (addr<0) return 0;
		ctb_addr_x = (u32) ( addr % si->pps->pic_width_in_ctbsY );
		ctb_addr_y = (u32) ( addr / si->pps->pic_width_in_ctbsY );

		ctb_to_tile_row_bd = vvc_ctb_to_tile_row_bd(si, ctb_addr_y);
		ctb_to_tile_col_bd = vvc_ctb_to_tile_col_bd(si, ctb_addr_x);

		if (i) {

			if ( ctb_to_tile_row_bd != prev_ctb_to_tile_row_bd
				|| ctb_to_tile_col_bd != prev_ctb_to_tile_col_bd
				|| ((ctb_addr_y != prev_ctb_addr_y) && si->sps->entropy_coding_sync_enabled_flag)
			) {
				nb_entry_points++;
			}

		}

		prev_ctb_addr_y = ctb_addr_y;
		prev_ctb_to_tile_row_bd = ctb_to_tile_row_bd;
		prev_ctb_to_tile_col_bd = ctb_to_tile_col_bd;
	}
	return nb_entry_points;
}