static void virtualdmabug_workaround(void)
{
	int hard_sectors;
	int end_sector;

	if (CT(raw_cmd->cmd[COMMAND]) == FD_WRITE) {
		raw_cmd->cmd[COMMAND] &= ~0x80;	/* switch off multiple track mode */

		hard_sectors = raw_cmd->length >> (7 + raw_cmd->cmd[SIZECODE]);
		end_sector = raw_cmd->cmd[SECTOR] + hard_sectors - 1;
		if (end_sector > raw_cmd->cmd[SECT_PER_TRACK]) {
			pr_info("too many sectors %d > %d\n",
				end_sector, raw_cmd->cmd[SECT_PER_TRACK]);
			return;
		}
		raw_cmd->cmd[SECT_PER_TRACK] = end_sector;
					/* make sure raw_cmd->cmd[SECT_PER_TRACK]
					 * points to end of transfer */
	}
}