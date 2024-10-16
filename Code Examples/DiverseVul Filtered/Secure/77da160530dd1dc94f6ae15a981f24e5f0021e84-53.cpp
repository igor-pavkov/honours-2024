struct hd_struct *disk_part_iter_next(struct disk_part_iter *piter)
{
	struct disk_part_tbl *ptbl;
	int inc, end;

	/* put the last partition */
	disk_put_part(piter->part);
	piter->part = NULL;

	/* get part_tbl */
	rcu_read_lock();
	ptbl = rcu_dereference(piter->disk->part_tbl);

	/* determine iteration parameters */
	if (piter->flags & DISK_PITER_REVERSE) {
		inc = -1;
		if (piter->flags & (DISK_PITER_INCL_PART0 |
				    DISK_PITER_INCL_EMPTY_PART0))
			end = -1;
		else
			end = 0;
	} else {
		inc = 1;
		end = ptbl->len;
	}

	/* iterate to the next partition */
	for (; piter->idx != end; piter->idx += inc) {
		struct hd_struct *part;

		part = rcu_dereference(ptbl->part[piter->idx]);
		if (!part)
			continue;
		if (!part_nr_sects_read(part) &&
		    !(piter->flags & DISK_PITER_INCL_EMPTY) &&
		    !(piter->flags & DISK_PITER_INCL_EMPTY_PART0 &&
		      piter->idx == 0))
			continue;

		get_device(part_to_dev(part));
		piter->part = part;
		piter->idx += inc;
		break;
	}

	rcu_read_unlock();

	return piter->part;
}