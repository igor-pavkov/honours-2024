static int check_duplicate_long_entries_update(TABLE *table, handler *h, uchar *new_rec)
{
  Field *field;
  uint key_parts;
  int error= 0;
  KEY *keyinfo;
  KEY_PART_INFO *keypart;
  /*
     Here we are comparing whether new record and old record are same
     with respect to fields in hash_str
   */
  uint reclength= (uint) (table->record[1] - table->record[0]);
  table->clone_handler_for_update();
  for (uint i= 0; i < table->s->keys; i++)
  {
    keyinfo= table->key_info + i;
    if (keyinfo->algorithm == HA_KEY_ALG_LONG_HASH)
    {
      key_parts= fields_in_hash_keyinfo(keyinfo);
      keypart= keyinfo->key_part - key_parts;
      for (uint j= 0; j < key_parts; j++, keypart++)
      {
        field= keypart->field;
        /* Compare fields if they are different then check for duplicates*/
        if(field->cmp_binary_offset(reclength))
        {
          if((error= check_duplicate_long_entry_key(table, table->update_handler,
                                                 new_rec, i)))
            goto exit;
          /*
            break because check_duplicate_long_entries_key will
            take care of remaining fields
           */
          break;
        }
      }
    }
  }
  exit:
  return error;
}