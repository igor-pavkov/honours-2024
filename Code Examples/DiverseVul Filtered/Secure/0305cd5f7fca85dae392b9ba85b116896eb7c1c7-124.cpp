static int __btrfs_subio_endio_read(struct inode *inode,
				    struct btrfs_io_bio *io_bio, int err)
{
	struct bio_vec *bvec;
	struct btrfs_retry_complete done;
	u64 start;
	u64 offset = 0;
	int i;
	int ret;

	err = 0;
	start = io_bio->logical;
	done.inode = inode;

	bio_for_each_segment_all(bvec, &io_bio->bio, i) {
		ret = __readpage_endio_check(inode, io_bio, i, bvec->bv_page,
					     0, start, bvec->bv_len);
		if (likely(!ret))
			goto next;
try_again:
		done.uptodate = 0;
		done.start = start;
		init_completion(&done.done);

		ret = dio_read_error(inode, &io_bio->bio, bvec->bv_page, start,
				     start + bvec->bv_len - 1,
				     io_bio->mirror_num,
				     btrfs_retry_endio, &done);
		if (ret) {
			err = ret;
			goto next;
		}

		wait_for_completion(&done.done);

		if (!done.uptodate) {
			/* We might have another mirror, so try again */
			goto try_again;
		}
next:
		offset += bvec->bv_len;
		start += bvec->bv_len;
	}

	return err;
}