void Ordered_key::print(String *str)
{
  uint i;
  str->append("{idx=");
  str->qs_append(keyid);
  str->append(", (");
  for (i= 0; i < key_column_count - 1; i++)
  {
    str->append(key_columns[i]->field->field_name);
    str->append(", ");
  }
  str->append(key_columns[i]->field->field_name);
  str->append("), ");

  str->append("null_bitmap: (bits=");
  str->qs_append(null_key.n_bits);
  str->append(", nulls= ");
  str->qs_append((double)null_count);
  str->append(", min_null= ");
  str->qs_append((double)min_null_row);
  str->append(", max_null= ");
  str->qs_append((double)max_null_row);
  str->append("), ");

  str->append('}');
}