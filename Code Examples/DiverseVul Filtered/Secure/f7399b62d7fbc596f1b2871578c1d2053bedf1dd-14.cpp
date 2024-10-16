static bool item_type_is_image(const std::string& item_type)
{
  return (item_type=="hvc1" ||
          item_type=="grid" ||
          item_type=="iden" ||
          item_type=="iovl");
}