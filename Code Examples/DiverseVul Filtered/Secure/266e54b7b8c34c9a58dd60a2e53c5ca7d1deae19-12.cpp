void imap_bodystructure_write(const struct message_part *part,
			      string_t *dest, bool extended)
{
	if ((part->flags & MESSAGE_PART_FLAG_MULTIPART) != 0)
		part_write_body_multipart(part, dest, extended);
	else
		part_write_body(part, dest, extended);
}