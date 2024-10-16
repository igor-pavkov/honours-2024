static const struct opcode_info *find_opcode_info(i64 opcode)
{
	size_t i;

	for(i=0; i<DE_ARRAYCOUNT(opcode_info_arr); i++) {
		if(opcode_info_arr[i].opcode == opcode) {
			return &opcode_info_arr[i];
		}
	}
	return NULL;
}