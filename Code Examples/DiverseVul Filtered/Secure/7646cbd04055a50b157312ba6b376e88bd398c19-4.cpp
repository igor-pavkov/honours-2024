int json_read_array(const char *cp, const struct json_array_t *arr,
		    const char **end)
{
    int substatus, offset, arrcount;
    char *tp;

    if (end != NULL)
	*end = NULL;	/* give it a well-defined value on parse failure */

    json_debug_trace((1, "Entered json_read_array()\n"));

    while (isspace((unsigned char) *cp))
	cp++;
    if (*cp != '[') {
	json_debug_trace((1, "Didn't find expected array start\n"));
	return JSON_ERR_ARRAYSTART;
    } else
	cp++;

    tp = arr->arr.strings.store;
    arrcount = 0;

    /* Check for empty array */
    while (isspace((unsigned char) *cp))
	cp++;
    if (*cp == ']')
	goto breakout;

    for (offset = 0; offset < arr->maxlen; offset++) {
#ifndef JSON_MINIMAL
	char *ep = NULL;
#endif /* JSON_MINIMAL */
	json_debug_trace((1, "Looking at %s\n", cp));
	switch (arr->element_type) {
	case t_string:
	    if (isspace((unsigned char) *cp))
		cp++;
	    if (*cp != '"')
		return JSON_ERR_BADSTRING;
	    else
		++cp;
	    arr->arr.strings.ptrs[offset] = tp;
	    for (; tp - arr->arr.strings.store < arr->arr.strings.storelen;
		 tp++)
		if (*cp == '"') {
		    ++cp;
		    *tp++ = '\0';
		    goto stringend;
		} else if (*cp == '\0') {
		    json_debug_trace((1,
				      "Bad string syntax in string list.\n"));
		    return JSON_ERR_BADSTRING;
		} else {
		    *tp = *cp++;
		}
	    json_debug_trace((1, "Bad string syntax in string list.\n"));
	    return JSON_ERR_BADSTRING;
	  stringend:
	    break;
	case t_object:
	case t_structobject:
	    substatus =
		json_internal_read_object(cp, arr->arr.objects.subtype, arr,
					  offset, &cp);
	    if (substatus != 0) {
#ifndef JSON_MINIMAL
		if (end != NULL)
		    end = &cp;
#endif /* JSON_MINIMAL */
		return substatus;
	    }
	    break;
	case t_integer:
#ifndef JSON_MINIMAL
	    arr->arr.integers.store[offset] = (int)strtol(cp, &ep, 0);
	    if (ep == cp)
		return JSON_ERR_BADNUM;
	    else
		cp = ep;
	    break;
#endif /* JSON_MINIMAL */
	case t_uinteger:
#ifndef JSON_MINIMAL
	    arr->arr.uintegers.store[offset] = (unsigned int)strtoul(cp,
                                                                     &ep, 0);
	    if (ep == cp)
		return JSON_ERR_BADNUM;
	    else
		cp = ep;
	    break;
#endif /* JSON_MINIMAL */
	case t_short:
#ifndef JSON_MINIMAL
	    arr->arr.shorts.store[offset] = (short)strtol(cp, &ep, 0);
	    if (ep == cp)
		return JSON_ERR_BADNUM;
	    else
		cp = ep;
	    break;
#endif /* JSON_MINIMAL */
	case t_ushort:
#ifndef JSON_MINIMAL
	    arr->arr.ushorts.store[offset] = (unsigned short)strtoul(cp,
                                                                     &ep, 0);
	    if (ep == cp)
		return JSON_ERR_BADNUM;
	    else
		cp = ep;
	    break;
#endif /* JSON_MINIMAL */
	case t_time:
#ifndef JSON_MINIMAL
	    if (*cp != '"')
		return JSON_ERR_BADSTRING;
	    else
		++cp;
	    arr->arr.reals.store[offset] = iso8601_to_unix((char *)cp);
	    if (arr->arr.reals.store[offset] >= HUGE_VAL)
		return JSON_ERR_BADNUM;
	    while (*cp && *cp != '"')
		cp++;
	    if (*cp != '"')
		return JSON_ERR_BADSTRING;
	    else
		++cp;
	    break;
#endif /* JSON_MINIMAL */
	case t_real:
#ifndef JSON_MINIMAL
	    arr->arr.reals.store[offset] = strtod(cp, &ep);
	    if (ep == cp)
		return JSON_ERR_BADNUM;
	    else
		cp = ep;
	    break;
#endif /* JSON_MINIMAL */
	case t_boolean:
#ifndef JSON_MINIMAL
	    if (str_starts_with(cp, "true")) {
		arr->arr.booleans.store[offset] = true;
		cp += 4;
	    }
	    else if (str_starts_with(cp, "false")) {
		arr->arr.booleans.store[offset] = false;
		cp += 5;
	    }
	    break;
#endif /* JSON_MINIMAL */
	case t_character:
	case t_array:
	case t_check:
	case t_ignore:
	    json_debug_trace((1, "Invalid array subtype.\n"));
	    return JSON_ERR_SUBTYPE;
	}
	arrcount++;
	if (isspace((unsigned char) *cp))
	    cp++;
	if (*cp == ']') {
	    json_debug_trace((1, "End of array found.\n"));
	    goto breakout;
	} else if (*cp == ',')
	    cp++;
	else {
	    json_debug_trace((1, "Bad trailing syntax on array.\n"));
	    return JSON_ERR_BADSUBTRAIL;
	}
    }
    json_debug_trace((1, "Too many elements in array.\n"));
#ifndef JSON_MINIMAL
    if (end != NULL)
	*end = cp;
#endif /* JSON_MINIMAL */
    return JSON_ERR_SUBTOOLONG;
  breakout:
    if (arr->count != NULL)
	*(arr->count) = arrcount;
    if (end != NULL)
	*end = cp;
    json_debug_trace((1, "leaving json_read_array() with %d elements\n",
		      arrcount));
    return 0;
}