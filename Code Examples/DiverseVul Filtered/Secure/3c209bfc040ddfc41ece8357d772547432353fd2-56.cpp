void Item_subselect::register_as_with_rec_ref(With_element *with_elem)
{
  with_elem->sq_with_rec_ref.link_in_list(this, &this->next_with_rec_ref);
  with_recursive_reference= true;
}