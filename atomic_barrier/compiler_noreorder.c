// compile cmd: gcc -S -O2 compiler_reorder.c
#define barrier() __asm__ __volatile__("" : : : "memory")
int a, b;
int i, j;

void main() {
    a = i;
    barrier();
    b = j / 16;
}
