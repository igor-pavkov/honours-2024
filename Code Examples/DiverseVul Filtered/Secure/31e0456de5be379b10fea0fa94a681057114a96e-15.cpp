static inline char *sms_get_fw_name(int mode, int board_id)
{
	char **fw = sms_get_board(board_id)->fw;
	return (fw && fw[mode]) ? fw[mode] : smsusb1_fw_lkup[mode];
}