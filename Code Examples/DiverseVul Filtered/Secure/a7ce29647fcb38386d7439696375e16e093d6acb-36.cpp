static const char *__int_type_from_size(int size) {
	switch (size) {
	case 1: return "int8_t";
	case 2: return "int16_t";
	case 4: return "int32_t";
	case 8: return "int64_t";
	default: return NULL;
	}
}