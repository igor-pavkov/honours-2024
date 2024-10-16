void *Type_Dictionary_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
   cmsHANDLE hDict;
   cmsUInt32Number i, Count, Length;
   cmsUInt32Number BaseOffset;
   _cmsDICarray a;
   wchar_t *NameWCS = NULL, *ValueWCS = NULL;
   cmsMLU *DisplayNameMLU = NULL, *DisplayValueMLU=NULL;
   cmsBool rc;

    *nItems = 0;

    // Get actual position as a basis for element offsets
    BaseOffset = io ->Tell(io) - sizeof(_cmsTagBase);

    // Get name-value record count
    if (!_cmsReadUInt32Number(io, &Count)) return NULL;
    SizeOfTag -= sizeof(cmsUInt32Number);

    // Get rec length
    if (!_cmsReadUInt32Number(io, &Length)) return NULL;
    SizeOfTag -= sizeof(cmsUInt32Number);

    // Check for valid lengths
    if (Length != 16 && Length != 24 && Length != 32) {
         cmsSignalError(self->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unknown record length in dictionary '%d'", Length);
         return NULL;
    }

    // Creates an empty dictionary
    hDict = cmsDictAlloc(self -> ContextID);
    if (hDict == NULL) return NULL;

    // On depending on record size, create column arrays
    if (!AllocArray(self -> ContextID, &a, Count, Length)) goto Error;

    // Read column arrays
    if (!ReadOffsetArray(io, &a, Count, Length, BaseOffset)) goto Error;

    // Seek to each element and read it
    for (i=0; i < Count; i++) {

        if (!ReadOneWChar(io, &a.Name, i, &NameWCS)) goto Error;
        if (!ReadOneWChar(io, &a.Value, i, &ValueWCS)) goto Error;

        if (Length > 16) {
            if (!ReadOneMLUC(self, io, &a.DisplayName, i, &DisplayNameMLU)) goto Error;
        }

        if (Length > 24) {
            if (!ReadOneMLUC(self, io, &a.DisplayValue, i, &DisplayValueMLU)) goto Error;
        }

        if (NameWCS == NULL || ValueWCS == NULL) {
        
            cmsSignalError(self->ContextID, cmsERROR_CORRUPTION_DETECTED, "Bad dictionary Name/Value");        
            rc = FALSE;
        }
        else {

            rc = cmsDictAddEntry(hDict, NameWCS, ValueWCS, DisplayNameMLU, DisplayValueMLU);
        }

        if (NameWCS != NULL) _cmsFree(self ->ContextID, NameWCS);
        if (ValueWCS != NULL) _cmsFree(self ->ContextID, ValueWCS);
        if (DisplayNameMLU != NULL) cmsMLUfree(DisplayNameMLU);
        if (DisplayValueMLU != NULL) cmsMLUfree(DisplayValueMLU);

        if (!rc) goto Error;
    }

   FreeArray(&a);
   *nItems = 1;
   return (void*) hDict;

Error:
   FreeArray(&a);
   cmsDictFree(hDict);
   return NULL;
}