void f2fs_flush_merged_bios(struct f2fs_sb_info *sbi)
{
	f2fs_submit_merged_bio(sbi, DATA, WRITE);
	f2fs_submit_merged_bio(sbi, NODE, WRITE);
	f2fs_submit_merged_bio(sbi, META, WRITE);
}