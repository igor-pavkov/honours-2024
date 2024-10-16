void csync_file_update(const char *filename, const char *peername)
{
	struct stat st;
	SQL("Removing file from dirty db",
			"delete from dirty where filename = '%s' and peername = '%s'",
			url_encode(filename), peername);
	if ( lstat_strict(prefixsubst(filename), &st) != 0 || csync_check_pure(filename) ) {
		SQL("Removing file from file db",
			"delete from file where filename = '%s'",
			url_encode(filename));
	} else {
		const char *checktxt = csync_genchecktxt(&st, filename, 0);

		SQL("Deleting old record from file db",
			"DELETE FROM file WHERE filename = '%s'",
			url_encode(filename));

		SQL("Insert record to file db",
			"INSERT INTO file (filename, checktxt) values "
			"('%s', '%s')", url_encode(filename),
			url_encode(checktxt));
	}
}