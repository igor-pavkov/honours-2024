static void pdf14_free_mask_stack(pdf14_ctx *ctx, gs_memory_t *memory)
{
    pdf14_mask_t *mask_stack = ctx->mask_stack;

    if (mask_stack->rc_mask != NULL) {
        pdf14_mask_t *curr_mask = mask_stack;
        pdf14_mask_t *old_mask;
        while (curr_mask != NULL) {
            rc_decrement(curr_mask->rc_mask, "pdf14_free_mask_stack");
            old_mask = curr_mask;
            curr_mask = curr_mask->previous;
            gs_free_object(old_mask->memory, old_mask, "pdf14_free_mask_stack");
        }
    } else {
        gs_free_object(memory, mask_stack, "pdf14_free_mask_stack");
    }
    ctx->mask_stack = NULL;
}