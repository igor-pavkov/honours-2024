static u32 vvc_ctb_to_tile_row_bd(VVCSliceInfo *si, u32 ctb_addr_y)
{
	u32 i, tile_y = 0;
	u32 tile_row_bd_val = 0;

	for (i=0; i <= si->pps->pic_height_in_ctbsY; i++) {
		if (i == tile_row_bd_val + si->pps->tile_rows_height_ctb[tile_y]) {
			tile_row_bd_val += si->pps->tile_rows_height_ctb[tile_y];
			tile_y++;
		}
		if (ctb_addr_y == i) return tile_row_bd_val;
	}
	return 0;
}