static void sweep2old (lua_State *L, GCObject **p) {
  GCObject *curr;
  while ((curr = *p) != NULL) {
    if (iswhite(curr)) {  /* is 'curr' dead? */
      lua_assert(isdead(G(L), curr));
      *p = curr->next;  /* remove 'curr' from list */
      freeobj(L, curr);  /* erase 'curr' */
    }
    else {  /* all surviving objects become old */
      setage(curr, G_OLD);
      p = &curr->next;  /* go to next element */
    }
  }
}