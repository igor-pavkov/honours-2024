static UChar32 deleteCharRoutine(UChar32 c) {
    return thisKeyMetaCtrl | ctrlChar('H');
}  // key labeled Backspace