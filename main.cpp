﻿
// #ifdef WIN32
// #include <Windows.h>
// #else

// #endif
#include <Sprites.h>
#include <Backs.h>
#include <2d-lib.h>
#include <conio.h>
#include <string>
#define FPS 30.0
;



int main(int argc, char* argv[]) {
//restart:
	screen_init();
	#ifdef FPS
	double FRQ = 1.0/FPS;
	if (argc > 2) {
		std::string flag = argv[1], num = argv[2];
		// printf("%s\n%s\n", argv[1], argv[2]);
		if (flag == "-f" || flag == "-fps")
			FRQ = 1.0/double(std::stoi(num.c_str()));
		// printf("%f\n", FRQ);
		// getchar();
	}
	std::clock_t start;
	#endif
	state position = run2;
	int debug = 0;

	Character dino{ 10, 31, dino_bmp[position], position };
	Character cactus1{ 127 +  random, 31, enemy_bmp, run1 };
	Character cactus2{ 127 + 128 + random, 31, enemy_bmp, run1 };

	Back clouds{ sky, SKY_ROW };
	Back gnd{ ground, GND_ROW };
	
	int button = 0;
	short tick = 1;
	jump_handler(dino, RESET);
    
	clouds.print();
	gnd.print();
	// printf("%f\n\n", FRQ);
	// getch();
restart:
	// int score = 0;
	while (!(dino.check_hit(cactus1) || dino.check_hit(cactus2))) {

		#ifdef FPS
		start = std::clock();
		#endif

		tick = (++ tick) % 7;

		if (dino.row == 31 && tick == 6) dino.cond = state(!bool(dino.cond));
		else if (dino.row < 31) dino.cond = jump;
		else if (dino.row > 31) {
			printf("row %d - ERROR", dino.row);
			exit(1);
		}
		dino.bmp = dino_bmp[dino.cond];

		if (cactus1.col < -IMG_W + 1) cactus1.col = 127 + random;
		if (cactus2.col < -IMG_W + 1) cactus2.col = 127 + random;

		// printScore(score);

		if (tick % 3 == 0) 
			clouds.offset().print();
		else
			clouds.print();
		gnd.offset().print();

		dino.print();
		cactus1.print();
		cactus2.print();

		display();
		// /*
		if (_kbhit()) {
		#ifdef WIN32
			_getch();
		#else
			getch();
		#endif
			button = 1;
		}
		// */
		jump_handler(dino.clear(), button);
		button = 0;

		// if (!(dino.check_hit(&cactus1)) || dino.check_hit(&cactus2))
			// break;

		cactus1.clear().col--;
		cactus2.clear().col--;

		#ifdef FPS
		while ((( std::clock() - start ) / (double) CLOCKS_PER_SEC) < FRQ)
			usleep(300);
		#endif

    	}
	
	// std::cout << duration << std::endl;
	cactus1.col = 127;
	cactus2.col = 127 + random + 64;
	jump_handler(dino, RESET);
	tick = 1;
	// goto restart;
	}