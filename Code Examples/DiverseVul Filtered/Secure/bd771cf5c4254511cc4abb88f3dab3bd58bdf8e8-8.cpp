static void smtc_set_timing(struct smtcfb_info *sfb)
{
	switch (sfb->chip_id) {
	case 0x710:
	case 0x712:
	case 0x720:
		sm7xx_set_timing(sfb);
		break;
	}
}