static void StringStart(enum string_t type)
{
	curr->w_StringType = type;
	curr->w_stringp = curr->w_string;
	curr->w_state = ASTR;
}