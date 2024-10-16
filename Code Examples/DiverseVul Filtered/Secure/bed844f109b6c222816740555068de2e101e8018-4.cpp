JsVar *jslNewStringFromLexer(JslCharPos *charFrom, size_t charTo) {
  // Original method - just copy it verbatim
  size_t maxLength = charTo + 1 - jsvStringIteratorGetIndex(&charFrom->it);
  assert(maxLength>0); // will fail if 0
  // Try and create a flat string first
  JsVar *var = 0;
  if (maxLength > JSV_FLAT_STRING_BREAK_EVEN) {
    var = jsvNewFlatStringOfLength((unsigned int)maxLength);
    if (var) {
      // Flat string
      char *flatPtr = jsvGetFlatStringPointer(var);
      *(flatPtr++) = charFrom->currCh;
      JsvStringIterator it = jsvStringIteratorClone(&charFrom->it);
      while (jsvStringIteratorHasChar(&it) && (--maxLength>0)) {
        *(flatPtr++) = jsvStringIteratorGetChar(&it);
        jsvStringIteratorNext(&it);
      }
      jsvStringIteratorFree(&it);
      return var;
    }
  }
  // Non-flat string...
  var = jsvNewFromEmptyString();
  if (!var) { // out of memory
    return 0;
  }

  //jsvAppendStringVar(var, lex->sourceVar, charFrom->it->index, (int)(charTo-charFrom));
  JsVar *block = jsvLockAgain(var);
  block->varData.str[0] = charFrom->currCh;
  size_t blockChars = 1;

  size_t l = maxLength;
  // now start appending
  JsvStringIterator it = jsvStringIteratorClone(&charFrom->it);
  while (jsvStringIteratorHasChar(&it) && (--maxLength>0)) {
    char ch = jsvStringIteratorGetChar(&it);
    if (blockChars >= jsvGetMaxCharactersInVar(block)) {
      jsvSetCharactersInVar(block, blockChars);
      JsVar *next = jsvNewWithFlags(JSV_STRING_EXT_0);
      if (!next) break; // out of memory
      // we don't ref, because  StringExts are never reffed as they only have one owner (and ALWAYS have an owner)
      jsvSetLastChild(block, jsvGetRef(next));
      jsvUnLock(block);
      block = next;
      blockChars=0; // it's new, so empty
    }
    block->varData.str[blockChars++] = ch;
    jsvStringIteratorNext(&it);
  }
  jsvSetCharactersInVar(block, blockChars);
  jsvUnLock(block);
  // Just make sure we only assert if there's a bug here. If we just ran out of memory or at end of string it's ok
  assert((l == jsvGetStringLength(var)) || (jsErrorFlags&JSERR_MEMORY) || !jsvStringIteratorHasChar(&it));
  jsvStringIteratorFree(&it);


  return var;
}