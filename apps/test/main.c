// Simple test exec firing the debug interrupt
void _start(void) {
  __asm__ volatile("int $1");
}
