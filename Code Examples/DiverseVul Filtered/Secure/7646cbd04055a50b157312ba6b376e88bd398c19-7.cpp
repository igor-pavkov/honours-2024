static char *json_target_address(const struct json_attr_t *cursor,
					     const struct json_array_t
					     *parent, int offset)
{
    char *targetaddr = NULL;
    if (parent == NULL || parent->element_type != t_structobject) {
	/* ordinary case - use the address in the cursor structure */
	switch (cursor->type) {
	case t_ignore:
	    targetaddr = NULL;
	    break;
	case t_integer:
	    targetaddr = (char *)&cursor->addr.integer[offset];
	    break;
	case t_uinteger:
	    targetaddr = (char *)&cursor->addr.uinteger[offset];
	    break;
	case t_short:
	    targetaddr = (char *)&cursor->addr.shortint[offset];
	    break;
	case t_ushort:
	    targetaddr = (char *)&cursor->addr.ushortint[offset];
	    break;
	case t_time:
	case t_real:
	    targetaddr = (char *)&cursor->addr.real[offset];
	    break;
	case t_string:
	    targetaddr = cursor->addr.string;
	    break;
	case t_boolean:
	    targetaddr = (char *)&cursor->addr.boolean[offset];
	    break;
	case t_character:
	    targetaddr = (char *)&cursor->addr.character[offset];
	    break;
	default:
	    targetaddr = NULL;
	    break;
	}
    } else
	/* tricky case - hacking a member in an array of structures */
	targetaddr =
	    parent->arr.objects.base + (offset * parent->arr.objects.stride) +
	    cursor->addr.offset;
    json_debug_trace((1, "Target address for %s (offset %d) is %p\n",
		      cursor->attribute, offset, targetaddr));
    return targetaddr;
}