int handler::ha_check_for_upgrade(HA_CHECK_OPT *check_opt)
{
  int error;
  KEY *keyinfo, *keyend;
  KEY_PART_INFO *keypart, *keypartend;

  if (table->s->incompatible_version)
    return HA_ADMIN_NEEDS_ALTER;

  if (!table->s->mysql_version)
  {
    /* check for blob-in-key error */
    keyinfo= table->key_info;
    keyend= table->key_info + table->s->keys;
    for (; keyinfo < keyend; keyinfo++)
    {
      keypart= keyinfo->key_part;
      keypartend= keypart + keyinfo->user_defined_key_parts;
      for (; keypart < keypartend; keypart++)
      {
        if (!keypart->fieldnr)
          continue;
        Field *field= table->field[keypart->fieldnr-1];
        if (field->type() == MYSQL_TYPE_BLOB)
        {
          if (check_opt->sql_flags & TT_FOR_UPGRADE)
            check_opt->flags= T_MEDIUM;
          return HA_ADMIN_NEEDS_CHECK;
        }
      }
    }
  }
  if (table->s->frm_version < FRM_VER_TRUE_VARCHAR)
    return HA_ADMIN_NEEDS_ALTER;

  if (unlikely((error= check_collation_compatibility())))
    return error;
    
  return check_for_upgrade(check_opt);
}