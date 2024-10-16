Compare_keys handler::compare_key_parts(const Field &old_field,
                                        const Column_definition &new_field,
                                        const KEY_PART_INFO &old_part,
                                        const KEY_PART_INFO &new_part) const
{
  if (!old_field.is_equal(new_field))
    return Compare_keys::NotEqual;

  if (old_part.length != new_part.length)
    return Compare_keys::NotEqual;

  return Compare_keys::Equal;
}