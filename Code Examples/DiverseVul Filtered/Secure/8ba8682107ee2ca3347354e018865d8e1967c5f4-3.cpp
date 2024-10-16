int ioprio_best(unsigned short aprio, unsigned short bprio)
{
	unsigned short aclass;
	unsigned short bclass;

	if (!ioprio_valid(aprio))
		aprio = IOPRIO_PRIO_VALUE(IOPRIO_CLASS_BE, IOPRIO_NORM);
	if (!ioprio_valid(bprio))
		bprio = IOPRIO_PRIO_VALUE(IOPRIO_CLASS_BE, IOPRIO_NORM);

	aclass = IOPRIO_PRIO_CLASS(aprio);
	bclass = IOPRIO_PRIO_CLASS(bprio);
	if (aclass == bclass)
		return min(aprio, bprio);
	if (aclass > bclass)
		return bprio;
	else
		return aprio;
}