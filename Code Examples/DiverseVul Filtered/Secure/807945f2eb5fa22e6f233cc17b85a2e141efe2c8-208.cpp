  longlong val_int() { return has_value() ? Date(this).to_longlong() : 0; }