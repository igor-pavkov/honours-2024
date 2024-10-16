int ha_init_errors(void)
{
#define SETMSG(nr, msg) handler_errmsgs[(nr) - HA_ERR_FIRST]= (msg)

  /* Allocate a pointer array for the error message strings. */
  /* Zerofill it to avoid uninitialized gaps. */
  if (! (handler_errmsgs= (const char**) my_malloc(HA_ERR_ERRORS * sizeof(char*),
                                                   MYF(MY_WME | MY_ZEROFILL))))
    return 1;

  /* Set the dedicated error messages. */
  SETMSG(HA_ERR_KEY_NOT_FOUND,          ER_DEFAULT(ER_KEY_NOT_FOUND));
  SETMSG(HA_ERR_FOUND_DUPP_KEY,         ER_DEFAULT(ER_DUP_KEY));
  SETMSG(HA_ERR_RECORD_CHANGED,         "Update which is recoverable");
  SETMSG(HA_ERR_WRONG_INDEX,            "Wrong index given to function");
  SETMSG(HA_ERR_CRASHED,                ER_DEFAULT(ER_NOT_KEYFILE));
  SETMSG(HA_ERR_WRONG_IN_RECORD,        ER_DEFAULT(ER_CRASHED_ON_USAGE));
  SETMSG(HA_ERR_OUT_OF_MEM,             "Table handler out of memory");
  SETMSG(HA_ERR_NOT_A_TABLE,            "Incorrect file format '%.64s'");
  SETMSG(HA_ERR_WRONG_COMMAND,          "Command not supported");
  SETMSG(HA_ERR_OLD_FILE,               ER_DEFAULT(ER_OLD_KEYFILE));
  SETMSG(HA_ERR_NO_ACTIVE_RECORD,       "No record read in update");
  SETMSG(HA_ERR_RECORD_DELETED,         "Intern record deleted");
  SETMSG(HA_ERR_RECORD_FILE_FULL,       ER_DEFAULT(ER_RECORD_FILE_FULL));
  SETMSG(HA_ERR_INDEX_FILE_FULL,        "No more room in index file '%.64s'");
  SETMSG(HA_ERR_END_OF_FILE,            "End in next/prev/first/last");
  SETMSG(HA_ERR_UNSUPPORTED,            ER_DEFAULT(ER_ILLEGAL_HA));
  SETMSG(HA_ERR_TO_BIG_ROW,             "Too big row");
  SETMSG(HA_WRONG_CREATE_OPTION,        "Wrong create option");
  SETMSG(HA_ERR_FOUND_DUPP_UNIQUE,      ER_DEFAULT(ER_DUP_UNIQUE));
  SETMSG(HA_ERR_UNKNOWN_CHARSET,        "Can't open charset");
  SETMSG(HA_ERR_WRONG_MRG_TABLE_DEF,    ER_DEFAULT(ER_WRONG_MRG_TABLE));
  SETMSG(HA_ERR_CRASHED_ON_REPAIR,      ER_DEFAULT(ER_CRASHED_ON_REPAIR));
  SETMSG(HA_ERR_CRASHED_ON_USAGE,       ER_DEFAULT(ER_CRASHED_ON_USAGE));
  SETMSG(HA_ERR_LOCK_WAIT_TIMEOUT,      ER_DEFAULT(ER_LOCK_WAIT_TIMEOUT));
  SETMSG(HA_ERR_LOCK_TABLE_FULL,        ER_DEFAULT(ER_LOCK_TABLE_FULL));
  SETMSG(HA_ERR_READ_ONLY_TRANSACTION,  ER_DEFAULT(ER_READ_ONLY_TRANSACTION));
  SETMSG(HA_ERR_LOCK_DEADLOCK,          ER_DEFAULT(ER_LOCK_DEADLOCK));
  SETMSG(HA_ERR_CANNOT_ADD_FOREIGN,     ER_DEFAULT(ER_CANNOT_ADD_FOREIGN));
  SETMSG(HA_ERR_NO_REFERENCED_ROW,      ER_DEFAULT(ER_NO_REFERENCED_ROW_2));
  SETMSG(HA_ERR_ROW_IS_REFERENCED,      ER_DEFAULT(ER_ROW_IS_REFERENCED_2));
  SETMSG(HA_ERR_NO_SAVEPOINT,           "No savepoint with that name");
  SETMSG(HA_ERR_NON_UNIQUE_BLOCK_SIZE,  "Non unique key block size");
  SETMSG(HA_ERR_NO_SUCH_TABLE,          "No such table: '%.64s'");
  SETMSG(HA_ERR_TABLE_EXIST,            ER_DEFAULT(ER_TABLE_EXISTS_ERROR));
  SETMSG(HA_ERR_NO_CONNECTION,          "Could not connect to storage engine");
  SETMSG(HA_ERR_TABLE_DEF_CHANGED,      ER_DEFAULT(ER_TABLE_DEF_CHANGED));
  SETMSG(HA_ERR_FOREIGN_DUPLICATE_KEY,  "FK constraint would lead to duplicate key");
  SETMSG(HA_ERR_TABLE_NEEDS_UPGRADE,    ER_DEFAULT(ER_TABLE_NEEDS_UPGRADE));
  SETMSG(HA_ERR_TABLE_READONLY,         ER_DEFAULT(ER_OPEN_AS_READONLY));
  SETMSG(HA_ERR_AUTOINC_READ_FAILED,    ER_DEFAULT(ER_AUTOINC_READ_FAILED));
  SETMSG(HA_ERR_AUTOINC_ERANGE,         ER_DEFAULT(ER_WARN_DATA_OUT_OF_RANGE));
  SETMSG(HA_ERR_TOO_MANY_CONCURRENT_TRXS, ER_DEFAULT(ER_TOO_MANY_CONCURRENT_TRXS));
  SETMSG(HA_ERR_INDEX_COL_TOO_LONG,	ER_DEFAULT(ER_INDEX_COLUMN_TOO_LONG));
  SETMSG(HA_ERR_INDEX_CORRUPT,		ER_DEFAULT(ER_INDEX_CORRUPT));
  SETMSG(HA_FTS_INVALID_DOCID,		"Invalid InnoDB FTS Doc ID");
  SETMSG(HA_ERR_TABLE_IN_FK_CHECK,	ER_DEFAULT(ER_TABLE_IN_FK_CHECK));
  SETMSG(HA_ERR_DISK_FULL,              ER_DEFAULT(ER_DISK_FULL));
  SETMSG(HA_ERR_FTS_TOO_MANY_WORDS_IN_PHRASE,  "Too many words in a FTS phrase or proximity search");
  SETMSG(HA_ERR_FK_DEPTH_EXCEEDED,      "Foreign key cascade delete/update exceeds");
  SETMSG(HA_ERR_TABLESPACE_MISSING,     ER_DEFAULT(ER_TABLESPACE_MISSING));

  /* Register the error messages for use with my_error(). */
  return my_error_register(get_handler_errmsgs, HA_ERR_FIRST, HA_ERR_LAST);
}