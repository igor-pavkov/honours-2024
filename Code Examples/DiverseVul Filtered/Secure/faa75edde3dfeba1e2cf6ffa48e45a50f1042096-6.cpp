static u32 lsr_to_dom_key(u32 lsr_k)
{
	switch (lsr_k) {
	case 0:
		return GF_KEY_STAR;
	case 1:
		return GF_KEY_0;
	case 2:
		return GF_KEY_1;
	case 3:
		return GF_KEY_2;
	case 4:
		return GF_KEY_3;
	case 5:
		return GF_KEY_4;
	case 6:
		return GF_KEY_5;
	case 7:
		return GF_KEY_6;
	case 8:
		return GF_KEY_7;
	case 9:
		return GF_KEY_8;
	case 10:
		return GF_KEY_9;
	case 12:
		return GF_KEY_DOWN;
	case 14:
		return GF_KEY_LEFT;
	case 16:
		return GF_KEY_RIGHT;
	case 20:
		return GF_KEY_UP;
	/*WHAT IS ANY_KEY (11) ??*/
	case 13:
		return GF_KEY_ENTER;
	case 15:
		return GF_KEY_ESCAPE;
	case 17:
		return GF_KEY_NUMBER;
	case 18:
		return GF_KEY_CELL_SOFT1;
	case 19:
		return GF_KEY_CELL_SOFT2;
	default:
		/*use '*' by default ... */
		return 0;
	}
}