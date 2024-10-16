static s32 vvc_get_ctb_info_in_slice(VVCSliceInfo *si, u32 sh_slice_address, u32 sh_num_tiles_in_slice, s32 ctu_index)
{
	if (si->pps->rect_slice_flag) {
/*
	TODO
		u32 picLevelSliceIdx = sh_slice_address;
		for (j=0; j<CurrSubpicIdx; j++) {
			picLevelSliceIdx += NumSlicesInSubpic[j];

		u32 NumCtusInCurrSlice = NumCtusInSlice[picLevelSliceIdx];
		for (i=0; i<NumCtusInCurrSlice; i++)
			CtbAddrInCurrSlice[i] = CtbAddrInSlice[ picLevelSliceIdx ][ i ]
*/

	} else {
		u32 i, tidx, NumCtusInCurrSlice = 0;

		for (tidx=sh_slice_address; tidx < sh_slice_address + sh_num_tiles_in_slice; tidx++) {
			u32 ctb_y, ctb_x;
			u32 tileX = tidx % si->pps->num_tile_cols;
			u32 tileY = tidx / si->pps->num_tile_cols;
			u32 min_ctbx=0;
			u32 max_ctbx=0;
			u32 min_ctby=0;
			u32 max_ctby=0;

			for (i=0; i<tileY; i++) min_ctby += si->pps->tile_rows_height_ctb[i];
			max_ctby = min_ctby + si->pps->tile_rows_height_ctb[i];

			for (i=0; i<tileX; i++) min_ctbx += si->pps->tile_cols_width_ctb[i];
			max_ctbx = min_ctbx + si->pps->tile_cols_width_ctb[i];


			for (ctb_y=min_ctby; ctb_y < max_ctby; ctb_y++) {
				for (ctb_x=min_ctbx; ctb_x < max_ctbx; ctb_x++) {
					if (ctu_index>=0) {
						if (ctu_index == NumCtusInCurrSlice)
							return ctb_y * si->pps->pic_width_in_ctbsY + ctb_x;
					}
					NumCtusInCurrSlice++;
				}
			}
		}
		if (ctu_index>=0) return -1;
		return NumCtusInCurrSlice;
	}
	return -1;
}