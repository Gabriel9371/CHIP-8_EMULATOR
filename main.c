#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include "SDL2/SDL_video.h"
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <SDL2/SDL.h>

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


  bool screen [32][64];

}Chip8;

void init(Chip8* cpu){

  memset(cpu, 0, sizeof(Chip8)); //troca lixo de memoria por 0 em CHIP8 ;)
  cpu->PC = 0x200;

}

int main(){
  Chip8 cpu;
  init(&cpu);

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow(
    "Janela",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    64, 32,
    SDL_WINDOW_SHOWN
  );
 SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  FILE* f = fopen("draw_test.ch8", "rb");
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
          memset(cpu.screen, 0, sizeof(cpu.screen));
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


      case 0xD:
      
        uint8_t vx = (opcode >> 8) & 0x000F;
        uint8_t vy = (opcode >> 4) & 0x000F;
        uint8_t n = opcode & 0x000F;

        cpu.reg[0xF] = 0;

        for(int i=0; i<n; i++){
          uint8_t byte_sprite = cpu.memory[cpu.I + i];
          for(int j=0; j<8; j++){

            if (byte_sprite & (0x80 >> j)) {
              uint8_t screen_x = cpu.reg[vx] + j;
              uint8_t screen_y = cpu.reg[vy] + i;

              if(cpu.screen[screen_y][screen_x] == true){
              
                cpu.reg[0xF] = 1;

              }
              cpu.screen[screen_y][screen_x] ^= true;
            }
          }
        }

      break;

    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int i=0; i<32; i++){
      for(int j=0; j<64; j++){
        SDL_Rect pixel = {j, i, 1, 1};
        if(cpu.screen[i][j] == true){
          SDL_RenderFillRect(renderer, &pixel);
        }
      }
    }
    
    SDL_RenderPresent(renderer);


    printf("PC: 0x%04X | Opcode: 0x%04X | V0: %d | I: 0x%04X\n", cpu.PC, opcode, cpu.reg[0], cpu.I);
    if(opcode == 0x0000){
      SDL_Delay(5000);
      runnig = true;

    }
  }
  printf("end execution. V0 = %d, I = 0x%04X\n", cpu.reg[0], cpu.I);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
