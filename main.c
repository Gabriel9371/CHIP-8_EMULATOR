#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct{
  uint8_t memory[4096];
  uint8_t reg[16];
  uint16_t I;

  uint16_t PC;

  uint16_t stack[16];
  uint8_t SP;

  uint8_t delay_timer;
  uint8_t sound_timer;

  bool keyboard[16];

}Chip8;

void init(Chip8* cpu){

  memset(cpu, 0, sizeof(Chip8)); //troca lixo de memoria por 0 em CHIP8 ;)
  cpu->PC = 0x200;

}

int main(){
  Chip8 cpu;
  init(&cpu);

  return 0;
}
