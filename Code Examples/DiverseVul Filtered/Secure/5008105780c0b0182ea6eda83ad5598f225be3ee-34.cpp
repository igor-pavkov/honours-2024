static void addjump(JF, enum js_AstType type, js_Ast *target, int inst)
{
	js_JumpList *jump = js_malloc(J, sizeof *jump);
	jump->type = type;
	jump->inst = inst;
	jump->next = target->jumps;
	target->jumps = jump;
}