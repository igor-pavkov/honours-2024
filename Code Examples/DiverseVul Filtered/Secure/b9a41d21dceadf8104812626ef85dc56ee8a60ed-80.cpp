void dm_remap_zone_report(struct dm_target *ti, struct bio *bio, sector_t start)
{
#ifdef CONFIG_BLK_DEV_ZONED
	struct dm_target_io *tio = container_of(bio, struct dm_target_io, clone);
	struct bio *report_bio = tio->io->bio;
	struct blk_zone_report_hdr *hdr = NULL;
	struct blk_zone *zone;
	unsigned int nr_rep = 0;
	unsigned int ofst;
	struct bio_vec bvec;
	struct bvec_iter iter;
	void *addr;

	if (bio->bi_status)
		return;

	/*
	 * Remap the start sector of the reported zones. For sequential zones,
	 * also remap the write pointer position.
	 */
	bio_for_each_segment(bvec, report_bio, iter) {
		addr = kmap_atomic(bvec.bv_page);

		/* Remember the report header in the first page */
		if (!hdr) {
			hdr = addr;
			ofst = sizeof(struct blk_zone_report_hdr);
		} else
			ofst = 0;

		/* Set zones start sector */
		while (hdr->nr_zones && ofst < bvec.bv_len) {
			zone = addr + ofst;
			if (zone->start >= start + ti->len) {
				hdr->nr_zones = 0;
				break;
			}
			zone->start = zone->start + ti->begin - start;
			if (zone->type != BLK_ZONE_TYPE_CONVENTIONAL) {
				if (zone->cond == BLK_ZONE_COND_FULL)
					zone->wp = zone->start + zone->len;
				else if (zone->cond == BLK_ZONE_COND_EMPTY)
					zone->wp = zone->start;
				else
					zone->wp = zone->wp + ti->begin - start;
			}
			ofst += sizeof(struct blk_zone);
			hdr->nr_zones--;
			nr_rep++;
		}

		if (addr != hdr)
			kunmap_atomic(addr);

		if (!hdr->nr_zones)
			break;
	}

	if (hdr) {
		hdr->nr_zones = nr_rep;
		kunmap_atomic(hdr);
	}

	bio_advance(report_bio, report_bio->bi_iter.bi_size);

#else /* !CONFIG_BLK_DEV_ZONED */
	bio->bi_status = BLK_STS_NOTSUPP;
#endif
}