static void ext4_unregister_li_request(struct super_block *sb)
{
	struct ext4_li_request *elr = EXT4_SB(sb)->s_li_request;

	if (!ext4_li_info)
		return;

	mutex_lock(&ext4_li_info->li_list_mtx);
	ext4_remove_li_request(elr);
	mutex_unlock(&ext4_li_info->li_list_mtx);
}