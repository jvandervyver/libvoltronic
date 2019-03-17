#include "axpert_crc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  axpert_crc_t crc;
  crc = calculate_axpert_crc("QPI", 3);
  printf("QPI=0x%04X\n", crc);
  crc = calculate_axpert_crc("QPIGS", 5);
  printf("QPIGS=0x%04X\n", crc);
}
