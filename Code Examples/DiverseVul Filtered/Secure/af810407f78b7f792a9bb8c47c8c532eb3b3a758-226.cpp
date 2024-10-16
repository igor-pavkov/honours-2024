int handler::check_collation_compatibility()
{
  ulong mysql_version= table->s->mysql_version;

  if (mysql_version < 50124)
  {
    KEY *key= table->key_info;
    KEY *key_end= key + table->s->keys;
    for (; key < key_end; key++)
    {
      KEY_PART_INFO *key_part= key->key_part;
      KEY_PART_INFO *key_part_end= key_part + key->user_defined_key_parts;
      for (; key_part < key_part_end; key_part++)
      {
        if (!key_part->fieldnr)
          continue;
        Field *field= table->field[key_part->fieldnr - 1];
        uint cs_number= field->charset()->number;
        if ((mysql_version < 50048 &&
             (cs_number == 11 || /* ascii_general_ci - bug #29499, bug #27562 */
              cs_number == 41 || /* latin7_general_ci - bug #29461 */
              cs_number == 42 || /* latin7_general_cs - bug #29461 */
              cs_number == 20 || /* latin7_estonian_cs - bug #29461 */
              cs_number == 21 || /* latin2_hungarian_ci - bug #29461 */
              cs_number == 22 || /* koi8u_general_ci - bug #29461 */
              cs_number == 23 || /* cp1251_ukrainian_ci - bug #29461 */
              cs_number == 26)) || /* cp1250_general_ci - bug #29461 */
             (mysql_version < 50124 &&
             (cs_number == 33 || /* utf8_general_ci - bug #27877 */
              cs_number == 35))) /* ucs2_general_ci - bug #27877 */
          return HA_ADMIN_NEEDS_UPGRADE;
      }
    }
  }

  return 0;
}