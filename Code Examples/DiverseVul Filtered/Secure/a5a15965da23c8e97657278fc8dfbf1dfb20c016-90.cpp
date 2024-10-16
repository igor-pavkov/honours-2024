SPL_METHOD(SplFileObject, getCsvControl)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	char delimiter[2], enclosure[2], escape[2];

	array_init(return_value);

	delimiter[0] = intern->u.file.delimiter;
	delimiter[1] = '\0';
	enclosure[0] = intern->u.file.enclosure;
	enclosure[1] = '\0';
	escape[0] = intern->u.file.escape;
	escape[1] = '\0';

	add_next_index_string(return_value, delimiter);
	add_next_index_string(return_value, enclosure);
	add_next_index_string(return_value, escape);
}