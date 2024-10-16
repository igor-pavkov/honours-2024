PHP_METHOD(Phar, hasMetadata)
{
	PHAR_ARCHIVE_OBJECT();

	RETURN_BOOL(phar_obj->arc.archive->metadata != NULL);
}