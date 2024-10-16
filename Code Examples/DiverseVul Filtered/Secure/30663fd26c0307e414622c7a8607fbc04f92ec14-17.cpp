void restore_state_to_opc(CPUX86State *env, TranslationBlock *tb,
                          target_ulong *data)
{
    int cc_op = data[1];
    env->eip = data[0] - tb->cs_base;
    if (cc_op != CC_OP_DYNAMIC) {
        env->cc_op = cc_op;
    }
}