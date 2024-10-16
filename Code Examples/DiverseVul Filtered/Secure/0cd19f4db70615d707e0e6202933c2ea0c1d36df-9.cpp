static u32 vvc_ctb_to_tile_col_bd(VVCSliceInfo *si, u32 ctb_addr_x)
{
	u32 i, tile_x = 0;
	u32 tile_col_bd_val = 0;
	for (i=0; i <= si->pps->pic_width_in_ctbsY; i++) {
		if (i == tile_col_bd_val + si->pps->tile_cols_width_ctb[tile_x] ) {
			tile_col_bd_val += si->pps->tile_cols_width_ctb[tile_x];
			tile_x++;
		}
		if (ctb_addr_x == i) return tile_col_bd_val;
	}
	return 0;
}