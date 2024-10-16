static void part_write_body_multipart(const struct message_part *part,
				      string_t *str, bool extended)
{
	const struct message_part_data *data = part->data;

	i_assert(part->data != NULL);

	if (part->children != NULL)
		part_write_bodystructure_siblings(part->children, str, extended);
	else {
		/* no parts in multipart message,
		   that's not allowed. write a single
		   0-length text/plain structure */
		if (!extended)
			str_append(str, EMPTY_BODY);
		else
			str_append(str, EMPTY_BODYSTRUCTURE);
	}

	str_append_c(str, ' ');
	imap_append_string(str, data->content_subtype);

	if (!extended)
		return;

	/* BODYSTRUCTURE data */

	str_append_c(str, ' ');
	params_write(data->content_type_params,
		data->content_type_params_count, str, FALSE);

	part_write_bodystructure_common(data, str);
}