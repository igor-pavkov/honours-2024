static void warn_dirty_buffer(struct buffer_head *bh)
{
	printk(KERN_WARNING
	       "JBD2: Spotted dirty metadata buffer (dev = %pg, blocknr = %llu). "
	       "There's a risk of filesystem corruption in case of system "
	       "crash.\n",
	       bh->b_bdev, (unsigned long long)bh->b_blocknr);
}