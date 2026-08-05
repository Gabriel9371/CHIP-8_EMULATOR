#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

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

  FILE* f = fopen("test.ch8", "rb");
 if(f == NULL){
   printf("Erro: erro ao iniciar a rom");
  return 1;
 }
  fread(&cpu.memory[cpu.PC], 1, 4096 - 0x200, f); // 4096 - 0x200 = 3584
  
 fclose(f);
  
  
  //TEST 
  //TEMPORARY LOOP 
  bool runnig = false;
  while(!runnig){

    uint16_t opcode = cpu.memory[cpu.PC] << 8 | cpu.memory[cpu.PC + 1];
    cpu.PC += 2;

    uint16_t nibble01 = opcode >> 12;

  

    switch (nibble01) {

      case 0x0:
        if (opcode == 0x00E0) { // 00E0 CLEAR
          printf("Instrução de limpar a tela"); //TODO futuramente

        }else if(opcode == 0x00EE){
          cpu.SP--;
          cpu.PC = cpu.stack[cpu.SP];
        }
       break;

      case 0x1:
        cpu.PC = opcode & 0x0FFF; //
        break;

      case 0x6:
        uint8_t x = (opcode >> 8) & 0x000F;
        uint8_t nn = opcode & 0x00FF;

        cpu.reg[x] = nn;
        break;

      case 0x7:
        uint8_t a = (opcode >> 8) & 0x000F;
        uint8_t b = opcode & 0x00FF;

        cpu.reg[a] += b;
        break;

      case 0xA:
        cpu.I = opcode & 0x0FFF;

        break;

      case 0x3:
        uint8_t tx = (opcode >> 8) & 0x000F;
        uint8_t tnn = opcode & 0x00FF;

        if(cpu.reg[tx] == tnn){
          cpu.PC += 2;
        }

        break;

      case 0x4:
        uint8_t ux = (opcode >> 8) & 0x000F;
        uint8_t unn = opcode & 0x00FF;
        if(cpu.reg[ux] != unn){
          cpu.PC += 2;
        }

        break;

      case 0x8:
    
        uint8_t xx = (opcode >> 8) & 0x000F;
        uint8_t yy = (opcode >> 4) & 0x000F;
        uint8_t op = opcode & 0x000F;

        if(op == 0x0){
          cpu.reg[xx] = cpu.reg[yy];
        
        }else if(op == 0x2){
          cpu.reg[xx] = cpu.reg[xx] & cpu.reg[yy];
        }

        break;

      case 0x2:

        cpu.stack[cpu.SP] = cpu.PC;
        cpu.SP++;

        cpu.PC = opcode & 0x0FFF;

        break;

    }
    printf("PC: 0x%04X | Opcode: 0x%04X | V0: %d | I: 0x%04X\n", cpu.PC, opcode, cpu.reg[0], cpu.I);
    if(opcode == 0x0000){
      runnig = true;

    }
  }
  printf("end execution. V0 = %d, I = 0x%04X\n", cpu.reg[0], cpu.I);
  return 0;
}
