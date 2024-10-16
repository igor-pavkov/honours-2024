static inline QuantumAny ScaleQuantumToAny(const Quantum quantum,
  const QuantumAny range)
{
  return((QuantumAny) (((double) range*quantum)/QuantumRange+0.5));
}