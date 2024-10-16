build_charclass (RE_TRANSLATE_TYPE trans, bitset_t sbcset,
		 const char *class_name, reg_syntax_t syntax)
#endif /* not RE_ENABLE_I18N */
{
  int i;
  const char *name = class_name;

  /* In case of REG_ICASE "upper" and "lower" match the both of
     upper and lower cases.  */
  if ((syntax & RE_ICASE)
      && (strcmp (name, "upper") == 0 || strcmp (name, "lower") == 0))
    name = "alpha";

#ifdef RE_ENABLE_I18N
  /* Check the space of the arrays.  */
  if (BE (*char_class_alloc == mbcset->nchar_classes, 0))
    {
      /* Not enough, realloc it.  */
      /* +1 in case of mbcset->nchar_classes is 0.  */
      Idx new_char_class_alloc = 2 * mbcset->nchar_classes + 1;
      /* Use realloc since array is NULL if *alloc == 0.  */
      wctype_t *new_char_classes = re_realloc (mbcset->char_classes, wctype_t,
					       new_char_class_alloc);
      if (BE (new_char_classes == NULL, 0))
	return REG_ESPACE;
      mbcset->char_classes = new_char_classes;
      *char_class_alloc = new_char_class_alloc;
    }
  mbcset->char_classes[mbcset->nchar_classes++] = __wctype (name);
#endif /* RE_ENABLE_I18N */

#define BUILD_CHARCLASS_LOOP(ctype_func)	\
  do {						\
    if (BE (trans != NULL, 0))			\
      {						\
	for (i = 0; i < SBC_MAX; ++i)		\
	  if (ctype_func (i))			\
	    bitset_set (sbcset, trans[i]);	\
      }						\
    else					\
      {						\
	for (i = 0; i < SBC_MAX; ++i)		\
	  if (ctype_func (i))			\
	    bitset_set (sbcset, i);		\
      }						\
  } while (0)

  if (strcmp (name, "alnum") == 0)
    BUILD_CHARCLASS_LOOP (isalnum);
  else if (strcmp (name, "cntrl") == 0)
    BUILD_CHARCLASS_LOOP (iscntrl);
  else if (strcmp (name, "lower") == 0)
    BUILD_CHARCLASS_LOOP (islower);
  else if (strcmp (name, "space") == 0)
    BUILD_CHARCLASS_LOOP (isspace);
  else if (strcmp (name, "alpha") == 0)
    BUILD_CHARCLASS_LOOP (isalpha);
  else if (strcmp (name, "digit") == 0)
    BUILD_CHARCLASS_LOOP (isdigit);
  else if (strcmp (name, "print") == 0)
    BUILD_CHARCLASS_LOOP (isprint);
  else if (strcmp (name, "upper") == 0)
    BUILD_CHARCLASS_LOOP (isupper);
  else if (strcmp (name, "blank") == 0)
    BUILD_CHARCLASS_LOOP (isblank);
  else if (strcmp (name, "graph") == 0)
    BUILD_CHARCLASS_LOOP (isgraph);
  else if (strcmp (name, "punct") == 0)
    BUILD_CHARCLASS_LOOP (ispunct);
  else if (strcmp (name, "xdigit") == 0)
    BUILD_CHARCLASS_LOOP (isxdigit);
  else
    return REG_ECTYPE;

  return REG_NOERROR;
}