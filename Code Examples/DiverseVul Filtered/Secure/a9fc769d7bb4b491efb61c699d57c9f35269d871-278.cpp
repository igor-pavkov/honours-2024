const char* proto_field_display_to_string(int field_display)
{
	return val_to_str_const(field_display, hf_display, "Unknown");
}