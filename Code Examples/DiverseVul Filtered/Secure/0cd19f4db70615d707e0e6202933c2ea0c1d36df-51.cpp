static u32 gf_get_ue_nb_bits(u32 num) {
	s32 length = 1;
	s32 temp = ++num;

	while (temp != 1) {
		temp >>= 1;
		length += 2;
	}

	return (length >> 1) + ( (length + 1) >> 1);
}