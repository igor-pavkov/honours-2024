void ide_set_sector(IDEState *s, int64_t sector_num)
{
    unsigned int cyl, r;
    if (s->select & 0x40) {
	if (!s->lba48) {
            s->select = (s->select & 0xf0) | (sector_num >> 24);
            s->hcyl = (sector_num >> 16);
            s->lcyl = (sector_num >> 8);
            s->sector = (sector_num);
	} else {
	    s->sector = sector_num;
	    s->lcyl = sector_num >> 8;
	    s->hcyl = sector_num >> 16;
	    s->hob_sector = sector_num >> 24;
	    s->hob_lcyl = sector_num >> 32;
	    s->hob_hcyl = sector_num >> 40;
	}
    } else {
        cyl = sector_num / (s->heads * s->sectors);
        r = sector_num % (s->heads * s->sectors);
        s->hcyl = cyl >> 8;
        s->lcyl = cyl;
        s->select = (s->select & 0xf0) | ((r / s->sectors) & 0x0f);
        s->sector = (r % s->sectors) + 1;
    }
}