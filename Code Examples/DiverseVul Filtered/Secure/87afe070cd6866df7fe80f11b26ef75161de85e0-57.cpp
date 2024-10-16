void PrintTags()
{
	u32 i = 0;

	gf_sys_format_help(helpout, help_flags, "# Tagging support\n"
	"Tags are specified as a colon-separated list `tag_name=tag_value[:tag2=val2]`\n"
	"Setting a tag with no value or value `NULL` removes the tag.\n"
	"Special tag value `clear` (or `reset`) removes all tags.\n"
	"Unsupported tags can be added using their four character code as a tag name, and string value will be assumed.\n"
	"If the tag name length is 3, the prefix 0xA9 is used to create the four character code.\n"
	"  \n"
	"Tags can also be loaded from a text file using `-itags filename`. The file must be in UTF8 with:\n"
	"- lines starting with `tag_name=value` specify the start of a tag\n"
	"- other lines specify the remainder of the last declared tag\n"
	"  \n"
	"If tag name starts with `WM/`, the tag is added to `Xtra` box (WMA tag, string only).\n"
	"  \n"
	"Supported tag names, values, types, aliases:\n"
	);

	while (1) {
		s32 type = gf_itags_get_type(i);
		if (type<0) break;
		const char *name = gf_itags_get_name(i);
		u32 itag = gf_itags_get_itag(i);
		gf_sys_format_help(helpout, help_flags | GF_PRINTARG_HIGHLIGHT_FIRST , "%s", name);
		gf_sys_format_help(helpout, help_flags, " (%s) ", gf_4cc_to_str(itag) );
		switch (type) {
		case GF_ITAG_STR:
			gf_sys_format_help(helpout, help_flags, "string"); break;
		case GF_ITAG_INT8:
		case GF_ITAG_INT16:
		case GF_ITAG_INT32:
		case GF_ITAG_INT64:
			gf_sys_format_help(helpout, help_flags, "integer"); break;
		case GF_ITAG_FRAC6:
		case GF_ITAG_FRAC8:
			gf_sys_format_help(helpout, help_flags, "fraction (syntax: `A/B` or `A`, B will be 0)"); break;
		case GF_ITAG_BOOL:
			gf_sys_format_help(helpout, help_flags, "bool (`yes` or `no`)"); break;
		case GF_ITAG_ID3_GENRE:
			gf_sys_format_help(helpout, help_flags, "string (ID3 genre tag)"); break;
		case GF_ITAG_FILE:
			gf_sys_format_help(helpout, help_flags, "file path"); break;
		}
		name = gf_itags_get_alt_name(i);
		if (name) {
			gf_sys_format_help(helpout, help_flags, " (`alias` %s)", name);
		}

		gf_sys_format_help(helpout, help_flags, "\n");
		i++;
	}
}