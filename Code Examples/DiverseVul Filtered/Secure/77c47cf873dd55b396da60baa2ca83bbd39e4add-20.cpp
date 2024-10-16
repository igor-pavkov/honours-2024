static RAnalValue *anal_fill_im(RAnal *anal, st32 v) {
        RAnalValue *ret = r_anal_value_new ();
        ret->imm = v;
        return ret;
}