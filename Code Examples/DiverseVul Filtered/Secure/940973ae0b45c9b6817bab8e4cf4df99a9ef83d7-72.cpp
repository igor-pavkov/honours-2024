int64_t ide_get_sector(IDEState *s)
{
    int64_t sector_num;
    if (s->select & 0x40) {
        /* lba */
	if (!s->lba48) {
	    sector_num = ((s->select & 0x0f) << 24) | (s->hcyl << 16) |
		(s->lcyl << 8) | s->sector;
	} else {
	    sector_num = ((int64_t)s->hob_hcyl << 40) |
		((int64_t) s->hob_lcyl << 32) |
		((int64_t) s->hob_sector << 24) |
		((int64_t) s->hcyl << 16) |
		((int64_t) s->lcyl << 8) | s->sector;
	}
    } else {
        sector_num = ((s->hcyl << 8) | s->lcyl) * s->heads * s->sectors +
            (s->select & 0x0f) * s->sectors + (s->sector - 1);
    }
    return sector_num;
}