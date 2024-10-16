static RAnalValue *anal_fill_ai_rg(RAnal *anal, int idx) {
        RAnalValue *ret = r_anal_value_new ();
        ret->reg = r_reg_get (anal->reg, regs[idx], R_REG_TYPE_GPR);
        return ret;
}