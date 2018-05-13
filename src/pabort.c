#include <stdio.h>
#include <stdlib.h>

void pabort(const char *s)
{
  perror(s);
  abort();
}
