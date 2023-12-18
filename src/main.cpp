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

void printScore(int score, int initial_x = 0, int initial_y = 0) {
	std::string str_score = std::to_string(score);
	int space = initial_x;
	for (int i = 0; i < str_score.size(); i++) {
		std::string cur(1, str_score[i]);
		Sprite num(space, initial_y, fileToArray("res/" + cur + ".bmp"), background);
		num.print();
		space += 10;
	}
}

int get_distance(float speed) {
	return (std::rand() + 100) % (int)(speed * 100);
}

int convert_to_index(int cur) {
	if (cur >= 97) return cur - 86;
	else if (cur >= 65) return cur - 54;
	else return cur - 48;
}

void print_score_phrase() {
	int phrase[5] = { 83, 67, 79, 82, 69 };
	int space = 5;
	std::string cur_path;
	for (int i = 0; i < 5; i++) {
		cur_path = "res/alphabet/" + std::to_string(phrase[i]) + ".bmp";
		Sprite character(space, 5, fileToArray(cur_path), background);
		character.print();
		space += 6;
	}
}

void print_enter_phrase() {
	int phrase[8] = { 78, 73, 67, 75, 78, 65, 77, 69 };
	int space = 5;
	std::string cur_path;
	for (int i = 0; i < 8; i++) {
		cur_path = "res/alphabet/" + std::to_string(phrase[i]) + ".bmp";
		Sprite character(space, 30, fileToArray(cur_path), background);
		character.print();
		space += 6;
	}
}

void print_info_phrase() {
	int phrase[10] = { 69, 83, 67, 84, 79, 83, 84, 65, 82, 84 };
	int space = 24;
	std::string cur_path;
	for (int i = 0; i < 10; i++) {
		if (i == 3 || i == 5) {
			space += 10;
		}
		else {
			space += 6;
		}
		cur_path = "res/alphabet/" + std::to_string(phrase[i]) + ".bmp";
		Sprite character(space, 5, fileToArray(cur_path), background);
		character.print();
		
	}
}

void print_symbols(std::vector <int> nick, char** alphabet[36], int inital_x, int inital_y = 30, bool is_dyn = false) {
	
	for (int i = 0; i < nick.size(); i++) {
		if (is_dyn) {
			inital_x += 6;
		}
		Sprite symbol(inital_x, inital_y, fileToArray("res/0.bmp"), background);
		symbol.bmp = alphabet[nick[i]];
		symbol.print();
	}
}

void writeMapToFile(const std::map<std::string, int>& scores, const std::string& filename) {
	std::ofstream outFile(filename);

	if (outFile.is_open()) {
		for (const auto& entry : scores) {
			outFile << entry.first << " " << entry.second << "\n";
		}

		outFile.close();
	}
	else {
		exit(1);
	}
}

std::map<std::string, int> readMapFromFile(const std::string& filename) {
	std::map<std::string, int> result;
	std::ifstream inFile(filename);

	if (inFile.is_open()) {
		std::string line;
		while (std::getline(inFile, line)) {
			std::istringstream iss(line);
			std::string key;
			int value;

			if (iss >> key >> value) {
				result[key] = value;
			}
			else {
				exit(1);
			}
		}
		inFile.close();
	}
	else {
		exit(1);
	}

	return result;
}

std::vector<int> stringToAsciiVector(const std::string& str) {
	std::vector<int> result;
	for (char c : str) {
		result.push_back(static_cast<int>(c));
	}
	return result;
}

std::string asciiVectorToString(const std::vector<int>& asciiVector) {
	std::string result;
	for (int code : asciiVector) {
		result += static_cast<char>(code);
	}
	return result;
}

std::vector<v_pair> sortMap(std::map<std::string, int> map) {
	std::vector<v_pair> vec;
	std::vector<v_pair> res;
	std::copy(map.begin(),
		map.end(),
		std::back_inserter<std::vector<v_pair>>(vec));


	std::sort(vec.begin(), vec.end(),
		[](const v_pair& l, const v_pair& r)
		{
			if (l.second != r.second) {
				return l.second > r.second;
			}

			return l.first > r.first;
		});
	int end = vec.size() < 3 ? vec.size() : 3;
	for (int i = 0; i < end; i++) {
		res.push_back(vec[i]);
	}
	
	return res;
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
	char** alphabet[37];
	alphabet[37] = fileToArray("res/empty.bmp")->bmp;
	std::string cur_path;
	int cur;
	for (int i = 0; i <= 36; i++) {
		if (i < 10) {
			cur = i + 48;
			cur_path = "res/alphabet/" + std::to_string(cur) + ".bmp";
		} else if (i >= 11) {
			cur = i + 54;
			cur_path = "res/alphabet/" + std::to_string(cur) + ".bmp";
		}
		alphabet[i] = fileToArray(cur_path) -> bmp;
	};
	{	
		
		
		Sprite(0,0,fileToArray("res/startscreen.bmp"), background).print();
		Screen::display();
		if (_getch() == 27) { printf("Thanks for playing"); return 0; }
	}

restart:
	jump_handler(dino);
	Screen::clear();
	std::map<std::string, int> user_table = readMapFromFile("scores.txt");
	float score = 0;
	float speed_tick = 0;
	float speed = (int)speed_tick / 100 == 0 ? 1 : (int)speed_tick / 100 + speed_tick / 1000.0;
	float const speed_scale = 0.05;
	std::vector <int> local_nick = {};
	std::vector <int> ascii_nick = {};
	int space = 55;
	{	
		while (true) {
			Sprite empty(space, 30, fileToArray("res/empty.bmp"), enemy);

			if (_kbhit()) {
				int cur_btn = getch();
				if (cur_btn == 27) break;
				if ((cur_btn >= 48 && cur_btn <= 57) || (cur_btn >= 65 && cur_btn <= 90) || (cur_btn >= 97 && cur_btn <= 122)) {
					local_nick.push_back(convert_to_index(cur_btn));
					ascii_nick.push_back(cur_btn);
					empty.bmp = alphabet[local_nick[local_nick.size() - 1]];
					space += 6;

				}
				if (cur_btn == 47) {
					local_nick.pop_back();
					ascii_nick.pop_back();
					empty.bmp = alphabet[37];
					space -= 6;

				}
				
			}
			print_info_phrase();
			print_enter_phrase();
			print_symbols(local_nick, alphabet, space);
			empty.print();

			Screen::display();
			

		};
		Screen::clear();
	};
	
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
		// Printing characters
		dino.print();
		bird.print();
		cactusk1.print();
		cactusk2.print();

		// Displaying buffer
		Screen::display();

		// Jump and Down handling
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
	// Score logic
	std::string nickname_key = asciiVectorToString(ascii_nick);
	if (user_table.find(nickname_key) == user_table.end()) {
		user_table[nickname_key] = score;
	} else if (user_table[nickname_key] < score) {
		user_table[nickname_key] = score;
	}
	std::vector<v_pair> top_players = sortMap(user_table);
	writeMapToFile(user_table, "scores.txt");

	// Resetting all values
	cactusk1.col = Screen::width;
	cactusk2.col = Screen::width + random + (Screen::width / 2);
	dino.is_down = false;
	bird.col = cactusk2.col + 50;
	jump_handler(dino, RESET);
	tick = 1;
	{
		Screen::clear();
		print_score_phrase();
		printScore(score, 35, 5);
		int cur_player = 1;
		int cur_space = 20;
		for (auto const& pair : top_players) {
			std::vector<int> ascii_n = stringToAsciiVector(pair.first);
			std::vector<int> local_nickname;
			printScore(cur_player, 5, cur_space);
			
			for (int j = 0; j < ascii_n.size(); j++) {
				local_nickname.push_back(convert_to_index(ascii_n[j]));
			};
			print_symbols(local_nickname, alphabet, 13, cur_space, true);
			printScore(pair.second, 100, cur_space);
			cur_player += 1;
			cur_space += 10;
		}
		Screen::display();
		if (_getch() == 27) { printf("Thanks for playing"); return 0; }
	}
	// Restart
	goto restart;
	return 0;
}