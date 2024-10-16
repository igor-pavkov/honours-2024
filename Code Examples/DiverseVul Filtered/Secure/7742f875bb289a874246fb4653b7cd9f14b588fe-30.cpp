const char* get_backup_uuid(MYSQL *connection)
{
	if (!backup_uuid) {
		backup_uuid = read_mysql_one_value(connection, "SELECT UUID()");
	}
	return backup_uuid;
}