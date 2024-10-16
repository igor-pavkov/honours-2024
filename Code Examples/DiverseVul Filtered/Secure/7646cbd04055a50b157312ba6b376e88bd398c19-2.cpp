const char *json_error_string(int err)
{
    const char *errors[] = {
	"unknown error while parsing JSON",
	"non-whitespace when expecting object start",
	"non-whitespace when expecting attribute start",
	"unknown attribute name",
	"attribute name too long",
	"saw [ when not expecting array",
	"array element specified, but no [",
	"string value too long",
	"token value too long",
	"garbage while expecting comma or } or ]",
	"didn't find expected array start",
	"error while parsing object array",
	"too many array elements",
	"garbage while expecting array comma",
	"unsupported array element type",
	"error while string parsing",
	"check attribute not matched",
	"can't support strings in parallel arrays",
	"invalid enumerated value",
	"saw quoted value when expecting nonstring",
	"didn't see quoted value when expecting string",
	"other data conversion error",
	"unexpected null value or attribute pointer",
    };

    if (err <= 0 || err >= (int)(sizeof(errors) / sizeof(errors[0])))
	return errors[0];
    else
	return errors[err];
}