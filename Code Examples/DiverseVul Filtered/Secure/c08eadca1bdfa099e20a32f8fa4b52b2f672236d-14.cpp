void em28xx_setup_xc3028(struct em28xx *dev, struct xc2028_ctrl *ctl)
{
	memset(ctl, 0, sizeof(*ctl));

	ctl->fname   = XC2028_DEFAULT_FIRMWARE;
	ctl->max_len = 64;
	ctl->mts = em28xx_boards[dev->model].mts_firmware;

	switch (dev->model) {
	case EM2880_BOARD_EMPIRE_DUAL_TV:
	case EM2880_BOARD_HAUPPAUGE_WINTV_HVR_900:
	case EM2882_BOARD_TERRATEC_HYBRID_XS:
	case EM2880_BOARD_TERRATEC_HYBRID_XS:
	case EM2880_BOARD_TERRATEC_HYBRID_XS_FR:
	case EM2881_BOARD_PINNACLE_HYBRID_PRO:
	case EM2882_BOARD_ZOLID_HYBRID_TV_STICK:
		ctl->demod = XC3028_FE_ZARLINK456;
		break;
	case EM2880_BOARD_HAUPPAUGE_WINTV_HVR_900_R2:
	case EM2882_BOARD_PINNACLE_HYBRID_PRO_330E:
		ctl->demod = XC3028_FE_DEFAULT;
		break;
	case EM2880_BOARD_AMD_ATI_TV_WONDER_HD_600:
		ctl->demod = XC3028_FE_DEFAULT;
		ctl->fname = XC3028L_DEFAULT_FIRMWARE;
		break;
	case EM2883_BOARD_HAUPPAUGE_WINTV_HVR_850:
	case EM2883_BOARD_HAUPPAUGE_WINTV_HVR_950:
	case EM2880_BOARD_PINNACLE_PCTV_HD_PRO:
		/* FIXME: Better to specify the needed IF */
		ctl->demod = XC3028_FE_DEFAULT;
		break;
	case EM2883_BOARD_KWORLD_HYBRID_330U:
	case EM2882_BOARD_DIKOM_DK300:
	case EM2882_BOARD_KWORLD_VS_DVBT:
		ctl->demod = XC3028_FE_CHINA;
		ctl->fname = XC2028_DEFAULT_FIRMWARE;
		break;
	case EM2882_BOARD_EVGA_INDTUBE:
		ctl->demod = XC3028_FE_CHINA;
		ctl->fname = XC3028L_DEFAULT_FIRMWARE;
		break;
	default:
		ctl->demod = XC3028_FE_OREN538;
	}
}