#include <cstdlib>
#include <iostream>
#include <string>
#include <iomanip>
#include <windows.h>
#include <stdio.h>



//memória, buffer de leitura, hack do buffer de leitura
unsigned char memory[4096]; //workram do chip8 -- 4kb, 4096 bytes.
unsigned char screen[(64 * 32)+1]; //cada pixel da tela do chip8. 2048 pixels, com estado on/off.
unsigned char registroV[16]; //registros usados com propósito geral pela máquina.
unsigned short opcode; //buffer para operações bitwise em opcodes.
unsigned short ivar;
unsigned short progcount = 0;//contador do codigo
unsigned short pilha[16];
unsigned short pilhapos;
unsigned char delay_timer;
unsigned char beep_timer;
int output = 0;

unsigned short x;
unsigned short y;
unsigned short height;
unsigned short pixel;

using namespace std;

FILE * rom; //funciona


void openrom(){
	char filename[20];
entrar:
	cout << "Digite o nome da rom a ser aberta pelo ""emulador""" << endl;
	cin >> filename; //abre a merda da rom de acordo com o nome que tu escrever
	rom = fopen(filename, "rb"); //binary BITCH

	if (rom == NULL){
		cout << "Falha ao abrir imagem! Verifique o nome da ROM." << endl;//vê se a rom existe
		Sleep(4000);
		system("cls");
		goto entrar; //sai do programa se a rom for invalida
	}
	else{
		cout << "Rom selecionada com sucesso." << endl;
		fseek(rom, 0, SEEK_SET); //poe no começo do arquivo, e logo mais o buffer começa a cópia pra memória
		cout << "Limpando memoria inicializada..";
		for (int i = 0; i < 4096; i++){
			memory[i] = 0; //limpa a memória de dados sujos
		}
		Sleep(1000);
		cout << " Completado." << endl;
	}
}

void initcoutengine(){
	int i = 0;
	cout << "Inicializando cout based GHETTO rendering engine" << endl;
	Sleep(110);
	cout << "GHETTO Engine. Console at it's finest." << endl;
	Sleep(400);
	system("cls");
	system("color 1f");
	for (i = 0; i < (64 * 32); i++)
	{
		cout << (char)219;
		if (i % 64 == 0){
			cout << endl;
			screen[i] = 0;
		}
	}
	Sleep(30);
	cout << "Recomendado reconfigurar altura da janela do console." << endl;
	system("pause");
}

void chip8load(){
unsigned char chip8_fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	}; //Fonte interna do chip8

cout << "Copiando fonte interna do chip8 para memoria...";
for (int i = 0; i < 80; i++){
	memory[i] = chip8_fontset[i];
}
Sleep(100);
cout << "Copiado!" << endl;

cout << "Limpando registradorVE..";
for (int i = 0; i < 16; i++){
	registroV[i] = 0;
	cout << "registroV[" << i << "] = " <<(int)registroV[i] << endl;
}
Sleep(300);
cout << "Inicializado!" << endl;

opcode = 0;
ivar = 0;
progcount = 512; //Inicializa outras partes menores do hardware.
for (int i = 0; i < 16; i++){
	pilha[i] = 0;
}
pilhapos = 0;
cout << "Inicializando emulador.." << endl;
}
 
void drawscreen(){
	std::ios_base::sync_with_stdio(false);
	//setlocale(LC_ALL, "English");
	unsigned short x = (int) registroV[(opcode & 0x0F00) >> 8];
	unsigned short y = (int) registroV[(opcode & 0x00F0) >> 4];
	unsigned short height = (opcode & 0x000F);
	unsigned short pixel;
	
	
	registroV[0xF] = 0;
	for (int yline = 0; yline < height; yline++)
	{
		pixel = memory[ivar + yline];
		for (int xline = 0; xline < 8; xline++)
		{
			if ((pixel & (0x80 >> xline)) != 0)
			{
				if (screen[(x + xline + ((y + yline) * 64))] == 1)
				{
					registroV[0xF] = 1;
				}
				screen[x + xline + ((y + yline) * 64)] ^= 1;
			}
		}
	}

	system("cls");
	for (int i = 1; i <=2048; i++)
	{
		if (screen[i] == 1)
			cout << (char)219;
		else
			cout << (char)32;

		if (i % 64 == 0)
			cout << endl;
	}
	cout << endl;
	Sleep(3);
	//system("pause");


}

void chip8EMULATE(){
	//setlocale(LC_ALL, "Portuguese");
	system("cls");
	cout << "Disassembler start." << endl;
	do{
		opcode = memory[progcount] << 8 | memory[progcount+1];
		//cout <<"Opcode fetched : "<< hex << setw(4) << opcode << endl;
		//Sleep(10);
		switch ((opcode & 0xF000)){
		case 0x0000:
				switch (opcode & 0x00FF){
					case 0x00E0:
						for (int i = 1; i <= 2048; i++){
						screen[i] = 0;
						}
					//	cout << "GFX Clear!" << endl;
						progcount += 2;
					break;
			
					case 0x00EE: 
				//		cout << "Retornando para o endereco " <<dec<< (int)pilha[pilhapos - 1] << " na memoria." << endl;
						--pilhapos;
						progcount = pilha[pilhapos];
						progcount += 2;
					break;
				
				}break;
		case 0x1000: 
			//cout << "Pulando para o endereco " <<dec<< (int)(opcode & 0x0FFF) << " na memoria." << endl;
			progcount = (opcode & 0x0FFF);
			break;
		case 0x2000: 
			//cout << "Chamando subrotina no endereco " <<dec << (int)(opcode & 0x0FFF) << " na memoria. Salvado PC na pilha." << endl;
			pilha[pilhapos] = progcount;
			pilhapos++;
			progcount = ((opcode & 0x0FFF));
			//Sleep(100);
			break;
		case 0x3000: 
			if (registroV[(opcode & 0x0f00) >> 8] == (opcode & 0x00ff)){
				progcount += 4;
				//cout << "Pulando próxima instrução." << endl;
			}
			else{
				progcount += 2;
				//cout << "Prosseguindo com instrução." << endl;
			}
			break;
		case 0x4000: 
			if ((registroV[opcode & 0x0f00 >> 8]) != (opcode & 0x00ff)){
				progcount += 4;
				//cout << "Pulando próxima instrução." << endl;
			}
			else{
				progcount += 2;
				//cout << "Prosseguindo com instrução." << endl;
			}
			break;
		case 0x5000: 
			if (registroV[(opcode & 0x0f00) >> 8] == registroV[(opcode & 0x00f0) >> 4]){
				//cout << "Pulando próxima instrução." << endl;
				progcount += 4;
			}
			else{
				//cout << "Prosseguindo com instrucao." << endl;
				progcount += 2;
			}
				break;
		case 0x6000: 
			registroV[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			//cout << "Setando registro[" <<dec<< (int)((opcode & 0x0F00) >> 8) << "] para " << (opcode & 0x00FF) << endl;
			progcount += 2;
			break;
		case 0x7000: 
			registroV[(opcode & 0x0f00) >> 8] += opcode & 0x00ff;
			//cout << "Somando ao Registro [" << dec << (int)((opcode & 0x0F00) >> 8) << "] o valor " << (opcode & 0x00ff) << endl;
			progcount += 2;
			break;
		case (0x8000) :
			switch ((opcode & 0x000F)){
			case 0x0000:
				registroV[(opcode & 0x0F00) >> 8] = registroV[(opcode & 0x00F0) >> 4];
				progcount += 2;
				break;
			case 0x0001:
				registroV[(opcode & 0x0F00) >> 8] = registroV[(opcode & 0x0F00) >> 8] | registroV[(opcode & 0x00F0) >> 4];
				progcount += 2;
				break;
			case 0x0002:
				registroV[(opcode & 0x0F00) >> 8] &= registroV[(opcode & 0x00F0) >> 4];
				progcount += 2;
				break;
			case 0x0003: 
				registroV[(opcode & 0x0F00) >> 8] = registroV[(opcode & 0x0F00) >> 8] ^ registroV[(opcode & 0x00F0) >> 4];
				progcount += 2;
				break;
			case 0x0004:
				if (registroV[(opcode & 0x0F00) >> 8] + registroV[(opcode & 0x00F0) >> 4] > 10){
					registroV[0xF] = 1;
				}
				else{
					registroV[0xF] = 0;
				}
				progcount += 2;
				break;
			case 0x0005:
				if (registroV[(opcode & 0x00F0) >> 4] > registroV[(opcode & 0x0F00) >> 8])
					registroV[0xF] = 0;
				else
					registroV[0xF] = 1;
				registroV[(opcode & 0x0F00) >> 8] -= registroV[(opcode & 0x00F0) >> 4];
				progcount += 2;
				break;
			case 0x0006:
				registroV[0xF] = registroV[(opcode & 0x0F00) >> 8] & 0x1;
				registroV[(opcode & 0x0F00) >> 8] >>= 1;
				progcount += 2;
				break;
			case 0x0007:
				if (registroV[(opcode & 0x0F00) >> 8] > registroV[(opcode & 0x00F0) >> 4])	
					registroV[0xF] = 0; 
				else
					registroV[0xF] = 1;
				registroV[(opcode & 0x0F00) >> 8] = registroV[(opcode & 0x00F0) >> 4] - registroV[(opcode & 0x0F00) >> 8];
				progcount += 2;
				break;
			case 0x000E:
				registroV[0xF] = registroV[(opcode & 0x0F00) >> 8] >> 7;
				registroV[(opcode & 0x0F00) >> 8] <<= 1;
				progcount += 2;
				break;
			}break;
		case 0x9000: 
			if (registroV[(opcode & 0x0F00) >> 8] != registroV[(opcode & 0x00F0) >> 4])
				progcount += 4;
			else
				progcount += 2;
			break;
		case 0xA000: 
			ivar = opcode & 0x0fff;
			//cout << "iVar recebendo valor " << dec << (int)(opcode & 0x0FFF) << endl;
			progcount += 2;
			break;
		case 0xB000:
			progcount = (opcode & 0x0FFF) + registroV[0];
			//cout << dec << (opcode & 0x0fff) << endl;
			break;
		case 0xC000:
			registroV[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
			//cout << "Setando valor aleatório em RegistroV[" <<dec<<(int) ((opcode & 0x0F00) >> 8) << "]" << endl;
			progcount += 2;
			break;
		case 0xD000:
			//cout << "Desenhar Sprite na posição " << dec << (int)((opcode & 0x0F00) >> 8) << " x e posição " <<(int)((opcode & 0x00F0) >> 4) << " y." << endl;
			//cout << "Altura do sprite: " << dec << (int)(opcode & 0x000F) << endl;
			//system("pause");
			drawscreen();
			progcount += 2;
			//system("pause");
			break;
		case (0xe000) :
			switch ((opcode & 0x00FF)){
			case 0x009e:
				//cout << "Nao programado! Opcode EX9E" << endl;
				//cout << "KEY NOT PRESSED!" << endl;
				progcount += 4;
				//Sleep(600);
				break;
			case 0x00a1:
				//cout << "KEY NOT PRESSED!" << endl;
				progcount += 4;
				//cout << "Nao programado! Opcode EXA1" << endl; 
				//Sleep(600);
				break;
		}break;
		case 0xf000:
			switch ((opcode & 0x00FF)){
			case 0x0007:
				registroV[(opcode & 0x0F00) >> 8] = delay_timer;
				//cout << "Setando Registro [" << dec << (int)((opcode & 0x0F00) >> 8) << "] para o valor do Delay Timer." << endl;
				//cout << "Delay timer vale " << (int)delay_timer << endl;
				progcount += 2;
				break;
			case 0x000a:
				//cout << "Nao programado! Opcode FX0A" << endl;
				progcount += 0;
				break;
			case 0x0015:
				delay_timer = registroV[(opcode & 0x0F00) >> 8];
				//cout << "Delay timer setado para " << dec << (int)(registroV[(opcode & 0x0F00) >> 8]) << endl;
				progcount += 2;
				break;
			case 0x0018:
				beep_timer = registroV[(opcode & 0x0F00) >> 8];
				progcount += 2;
				break;
			case 0x001e: 
				//cout << "Função satânica." << endl;
				if (ivar + registroV[(opcode & 0x0f00) >> 8] > 0xFFF)
					registroV[0xF] = 1;
				else
					registroV[0xF] = 0;
				ivar += registroV[(opcode & 0x0F00) >> 8];
				progcount += 2;
				break;
			case 0x0029: 
				//cout << "Sprite operation" << endl;
				ivar = registroV[(opcode & 0x0F00) >> 8] * 0x5;
				progcount += 2;
				break;
			case 0x0033:
				//cout << "Executando função satânica que soma os paranauê na memória." << endl;
				memory[ivar] = registroV[(opcode & 0x0F00) >> 8] / 100;
				memory[ivar + 1] = (registroV[(opcode & 0x0F00) >> 8] / 10) % 10;
				memory[ivar + 2] = (registroV[(opcode & 0x0F00) >> 8] % 100) % 10;
				progcount += 2;
				break;
			case 0x0055:
				for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
					memory[ivar + i] = registroV[i];
				ivar += ((opcode & 0x0F00) >> 8) + 1;
				progcount += 2;
				break;
			case 0x0065: 
				for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++){
					registroV[i] = memory[i + ivar];
					//cout << "Memory[" <<dec<<(int) (i + ivar) << "] = " <<dec << (int)memory[i + ivar]<<endl;
					//cout << "registroV[" << i << "] set to " << (int)memory[i + ivar] << endl;
				}
				ivar += ((opcode & 0x0F00) >> 8) + 1;
				progcount += 2;
				break;
			}break;
		}
		//Sleep(600);
		
		if (delay_timer > 0)
			--delay_timer;
		
		/*if (output = 1){
			cout << "Opcode executed: " << hex << opcode << endl;
			cout << "Prox opcode: " << hex << noshowbase << (int)memory[progcount] << (int)memory[progcount + 1] << endl;
			cout << "Program Counter: " << dec << progcount << endl;
			cout << "IVar: " << dec << ivar << endl;
			for (int i = 0; i < 16; i++)
			{
				cout << "Register V[" << i << "] = " << (int)registroV[i] << "	 Pilha[" << i << "] = " << (int)pilha[i] << endl;
			}
			system("pause");
			system("cls");
		}
		//output = true;*/
		if (beep_timer > 0){
			//Beep(300, 120);
			--beep_timer;
		}
} while (progcount < 4096);


}



//======================================================Fim func

int main(int argc, char *argv[]){
	std::ios_base::sync_with_stdio(false);
	openrom();//escolhe a rom a ser "jogada"
	cout << "Inicializando buffer de leitura.. Size of buffer = short unsigned 16 bits" << endl;
	cout << "Copiando ROM para a memoria virtual..";
	
	for (int x = 512; x < 4096; x += 1){
		fread(&memory[x], 1, 1, rom); //nunca entenderei a merda do file operator.
		x += 1;
		fread(&memory[x], 1, 1, rom);
	}
	Sleep(700);
	cout << "Copiado!" << endl;
	for (int i = 512; i < 4096; i++){
		if (memory[i] != 0){
			cout << internal << setfill('0');
			cout << "Memory pos " << i << ": " << showbase<< hex << setw(4) <<(int)memory[i] << dec << endl;
			//Sleep(1);
		}
	}

	initcoutengine(); //Init the ULTRA POWERFUL COUT ENGINE for 4K resolution graphics

	chip8load(); //inicializa as variáveis restantes, escreve a base na memória, prepara para receber opcodes.
	Sleep(800);
	chip8EMULATE(); //emula o sistema.

	system("pause");
	return EXIT_SUCCESS;
}
