#include<2d/screen.hpp>
#include<2d/sprite.hpp>

void Screen::init() {

    _COORD coord;
	coord.X = Screen::width;
	coord.Y = Screen::height;
	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = coord.Y - 1;
	Rect.Right = coord.X - 1;
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleScreenBufferSize(Handle, coord);
	SetConsoleWindowInfo(Handle, TRUE, &Rect);

    Screen::console_handler = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); 

	SetConsoleActiveScreenBuffer(Screen::console_handler);

}

void Screen::pixel(int col, int row, char colour) {
	buffer[(row * width) + col] = colour;
}

void Screen::clear() {for (int i = 0; i < Screen::width * Screen::height; i++) Screen::buffer[i] = ' ';}

void Screen::display() {
    // Screen::buffer[Screen::width * Screen::height - 1] = '\0';
    WriteConsoleOutputCharacter(Screen::console_handler, (LPCSTR) Screen::buffer, Screen::width * Screen::height + 1, { 0, 0 }, &Screen::bytes_written);
}

image* fileToArray(std::string filename) {
    FILE * file = fopen(filename.c_str(), "rb");
	if (!file) { printf("%s does not exist", filename.c_str()); exit(1);}
    short d;
    fseek(file, 18, SEEK_SET);
    int width, height;
    fread(&width, 4, 1, file);
    fread(&height, 4, 1, file);
    fseek(file, 28, SEEK_SET);
    fread(&d, 2, 1, file);
    if (d != 1) { printf("\nERROR: wrong color depth in file %s\n",filename); exit(1); }

    // if (width != 128 && width != 16 || height != 16 && height != 64) {printf("ERROR: wrong resolution in file %s", filename.c_str()); exit(1);}

    char** array = new char*[height]; for(int i = 0; i < height; i++) array[i] = new char[width];

    fseek(file, 62, SEEK_SET);
    for (int h = height - 1; h >= 0; h--) {
        char byte, bytes_to_skip = (4 - ((int)std::ceil( (double)width / 8.0 ) % 4)) % 4;
        int bits_left;
		bits_left = width;
        while (bits_left > 0) {
            fread(&byte, 1, 1, file);
            int x = 8 - std::min(8, (int)bits_left);
            for(int bit_mask = std::pow(2,7); bit_mask >= std::pow(2, x); bit_mask /= 2)
                array[h][width - bits_left--] = byte & bit_mask ? ' ' : -37;
        }
        fseek(file, bytes_to_skip, SEEK_CUR);
    }
    fclose(file);

	image* output = new image;
	output->bmp = array; output->w = width; output->h = height;

    return crop(output);
}