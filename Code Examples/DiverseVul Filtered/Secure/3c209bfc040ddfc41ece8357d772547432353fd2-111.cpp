void Item_exists_subselect::init_length_and_dec()
{
  decimals= 0;
  max_length= 1;
  max_columns= engine->cols();
}