  Compound_Selector_Ptr Simple_Selector::unify_with(Compound_Selector_Ptr rhs)
  {
    for (size_t i = 0, L = rhs->length(); i < L; ++i)
    { if (to_string() == rhs->at(i)->to_string()) return rhs; }

    // check for pseudo elements because they are always last
    size_t i, L;
    bool found = false;
    if (typeid(*this) == typeid(Pseudo_Selector) || typeid(*this) == typeid(Wrapped_Selector))
    {
      for (i = 0, L = rhs->length(); i < L; ++i)
      {
        if ((Cast<Pseudo_Selector>((*rhs)[i]) || Cast<Wrapped_Selector>((*rhs)[i])) && (*rhs)[L-1]->is_pseudo_element())
        { found = true; break; }
      }
    }
    else
    {
      for (i = 0, L = rhs->length(); i < L; ++i)
      {
        if (Cast<Pseudo_Selector>((*rhs)[i]) || Cast<Wrapped_Selector>((*rhs)[i]))
        { found = true; break; }
      }
    }
    if (!found)
    {
      rhs->append(this);
      return rhs;
    }
    rhs->elements().insert(rhs->elements().begin() + i, this);
    return rhs;
  }