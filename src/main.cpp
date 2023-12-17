#include <2d/screen.hpp>
#include <2d/sprite.hpp>
#define FPS 60.0

// initializating global variables
int Screen::scale;
int Screen::height;
int Screen::width;
char* Screen::buffer;
int Screen::jump_tick;
int Screen::dino_default_row;
HANDLE Screen::console_handler;
DWORD Screen::bytes_written = 0;

void printScore(int score, int initial = 0) {
	std::string str_score = std::to_string(score);
	int space = 0;
	for (int i = 0; i < str_score.size(); i++) {
		std::string cur(1, str_score[i]);
		Sprite num(space, initial, fileToArray("res/" + cur + ".bmp"), background);
		num.print();
		space += 10;
	}
}
int get_distance(float speed) {
	return (std::rand() + 100) % (int)(speed * 100);
}


int main(int argc, char* argv[]) {
	// Handling console arguments
	// Handling console arguments
	double FRQ = 1.0 / FPS;
	std::clock_t start;
	// screen and game variables
	int height = 64, width = 128, scale = 1, button = 0, tick = 1;

	for (int i = 1; i < argc; i++) {
		if ((std::string)argv[i] == "-h") { printf("Usage: %s [-h | --help] [-f | --fps <FPS> (default = 60)] ", argv[0]); return 0; }
		else if ((std::string)argv[i] == "-f" || (std::string)argv[i] == "--fps") FRQ = 1.0 / atoi(argv[i + 1]);
	}

	// Screen initialization
	Screen::scale = scale;
	Screen::width = width;
	Screen::height = height;
	Screen::bytes_written = 0;
	Screen::init(width, height);

	// Sprite clouds(0, 5, fileToArray("res/clouds.bmp"), background);

	image* gnd_img = fileToArray("res/ground.bmp");
	Sprite gnd(0, Screen::height - 1 - gnd_img->h, gnd_img, background);

	char** dino_up_bmps[3] = { fileToArray("res/run1.bmp")->bmp, fileToArray("res/run2.bmp")->bmp, fileToArray("res/jump.bmp")->bmp };

	char** dino_down_bmps[2] = { fileToArray("res/run3.bmp")->bmp, fileToArray("res/run4.bmp")->bmp };

	Sprite dino(Screen::width / 12, 0, fileToArray("res/run1.bmp"), character);
	Screen::dino_default_row = Screen::height - 1 - dino.getResolution().second - gnd_img->h;

	Sprite cactusk1(width + random, 0, fileToArray("res/cactus.bmp"), enemy);
	cactusk1.row = height - 1 - cactusk1.getResolution().second - gnd_img->h;
	Sprite cactusk2(width * 1.5 + random, cactusk1.row, fileToArray("res/cactus.bmp"), enemy);
	cactusk2.row = height - 1 - cactusk2.getResolution().second - gnd_img->h;
	Sprite bird(cactusk2.col + 73, 26, fileToArray("res/bird.bmp"), enemy);
	// testImage(fileToArray("res/run1.bmp"));
	{	
		Sprite btn(0, 0, fileToArray("res/alphabet/48.bmp"), background);
		btn.print();
		while (true) {
			if (_kbhit()) {
				int cur_btn = getch();
				if (cur_btn == 27) break;
				std::string ascii_code = std::to_string(cur_btn);
				btn.bmp = fileToArray("res/alphabet/" + ascii_code + ".bmp")->bmp;
			};
			btn.print();

		};
		
		// Sprite(0,0,fileToArray("res/startscreen.bmp"), background).print();
		Screen::display();
		if (_getch() == 27) { printf("Thanks for playing"); return 0; }
	}

restart:
	jump_handler(dino);
	Screen::clear();
	float score = 0;
	float speed_tick = 0;
	float speed = (int)speed_tick / 100 == 0 ? 1 : (int)speed_tick / 100 + speed_tick / 1000.0;
	float const speed_scale = 0.05;
	while (!(dino.check_hit(bird) || dino.check_hit(cactusk1) || dino.check_hit(cactusk2))) {
		start = std::clock();
		score += speed_scale;
		speed_tick += speed_scale;
		speed = (int)speed_tick / 100 == 0 ? 1 : (int)speed_tick / 100 + speed_tick / 1000.0;
		tick = (tick + 1) % 13;


		if (dino.is_down && dino.row == Screen::dino_default_row && !(tick % 6)) dino.bmp = dino_down_bmps[!(tick % 12)];
		else if (dino.row == Screen::dino_default_row && !(tick % 6)) dino.bmp = dino_up_bmps[!(tick % 12)];
		else if (dino.row < Screen::dino_default_row) dino.bmp = dino_up_bmps[2];
		else if (dino.row > Screen::dino_default_row) { printf("ERROR: character row %d", dino.row); return 1; }

		// Respawning enemies
		if (cactusk1.col < -cactusk1.getResolution().first + 1) {
			cactusk1.col = Screen::width + cactusk2.col + get_distance(speed);
		}
		if (cactusk2.col < -cactusk2.getResolution().first + 1) {
			cactusk2.col = Screen::width + cactusk1.col + get_distance(speed);
			if (cactusk1.col > 128 && get_distance(speed) % 3 == 0) {
				cactusk2.col = cactusk1.col + 10;
				if (bird.col < -bird.getResolution().first + 1) {
					bird.col = cactusk1.col + 124;
				}
			}
			else if (cactusk1.col > 128) {
				cactusk2.col = cactusk1.col + get_distance(speed) + 100;
				;
			}
		}

		// Printing and ofsetting backs
		// if (tick % 3 == 0) clouds.offset();
		// clouds.print();
		gnd.offset().print();
		printScore((int)score);
		//printScore(get_distance(speed), 12);
		// Printing characters
		dino.print();
		bird.print();
		cactusk1.print();
		cactusk2.print();

		// Displaying buffer
		Screen::display();

		// Jump handling
		if (_kbhit()) {
			switch (getch())
			{
			case 87:
			case 119:
				dino.is_down = false;
				button = 1;
				break;

			case 83:
			case 115:
				dino.is_down = true;
				Screen::jump_tick = 0;
				break;

			case 27:
				printf("Thanks for playing");
				exit(1);
				break;
			}

		}
		jump_handler(dino.clear(), button); button = 0;

		// Moving enemies
		cactusk1.clear().col -= speed;
		cactusk2.clear().col -= speed;
		bird.clear().col -= speed;

		// Framerate control

		while (((std::clock() - start) / (double)CLOCKS_PER_SEC) < FRQ);

	}

	// Resetting all values
	cactusk1.col = Screen::width;
	cactusk2.col = Screen::width + random + (Screen::width / 2);
	bird.col = cactusk2.col + 50;
	jump_handler(dino, RESET);
	tick = 1;
	{
		Sprite(0, 0, fileToArray("res/endscreen.bmp"), background).print();
		Screen::display();
		if (_getch() == 27) { printf("Thanks for playing"); return 0; }
	}
	// Restart
	goto restart;
	return 0;
}