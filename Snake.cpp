#include <windows.h>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <conio.h>
#define notDEBUG

class Snake_Engine {
private:

	class Console_Color {
	private:
		HANDLE h;
		int Color_ID;
	public:
		Console_Color() {
			h = GetStdHandle(STD_OUTPUT_HANDLE);
			Color_ID = 15;
		}
		void Set_Color(int Color_ID) {
			if (Color_ID >= 0 && Color_ID <= 15) {
				this->Color_ID = Color_ID;
				SetConsoleTextAttribute(h, this->Color_ID * 17);
			}
		}
		void Reset_Color() {
			SetConsoleTextAttribute(h, 15);
		}
	};

	class Play_Time {
	private:
		int start_time;
		int play_time;
		int hours;
		int minuts;
		int seconds;
		void Check_Zero(int value) {
			value < 10 ? std::cout << '0' : std::cout << "";
		}
		void Time_Turn_Into() {
			play_time = time(NULL) - start_time;
			hours = (play_time / 3600) % 24;
			minuts = (play_time % 3600) / 60;
			seconds = play_time % 60;
		}
	public:
		Play_Time() {
			start_time = time(NULL);
			play_time = 0;
			hours = 0;
			minuts = 0;
			seconds = 0;
		}
		void Print_Play_Time() {
			Time_Turn_Into();
			std::cout << std::endl << "Play time: ";
			Check_Zero(hours);
			std::cout << hours << ':';
			Check_Zero(minuts);
			std::cout << minuts << ':';
			Check_Zero(seconds);
			std::cout << seconds << std::endl;
		}
	};

	class Point {
	private:
		int x;
		int y;
	public:
		Point() {
			x = 0;
			y = 0;
		}
		int Get_x() {
			return x;
		}
		void Set_x(int value) {
			x = value;
		}
		int Get_y() {
			return y;
		}
		void Set_y(int value) {
			y = value;
		}
		void operator = (const Point& other) {
			this->x = other.x;
			this->y = other.y;
		}
	};

	Play_Time play_timer;
	Console_Color color;
	bool color_mode, object_code = false; 

	int step_x = 2, step_y = 1, tail_length = 3, food_amount = 20;
	const int const_tail_length = tail_length;
	static const int size_x = 65, size_y = 32;

	Point head_point;
	Point* tail = new Point[tail_length + 1];

	char vertical_border = '|', horizontal_border = '-', corner = '+', inside = ' ', object_head = '@', object_tail = '#', food = 'O';
	char field[size_y][size_x];
	char keyboard_list[8]{ 'w', 'a', 's', 'd', 'W', 'A', 'S', 'D' };
	char ch = keyboard_list[rand() % 8];

	void Spawn_Food() {
		srand(time(NULL));
		int food_counter = 0;
		for (int y = 0; y < size_y; y++) {
			for (int x = 0; x < size_x; x++) {
				if (field[y][x] == food) {
					food_counter++;
				}
			}
		}
		if (food_amount - food_counter >= 1) {
			Increase_Tail();
		}
		for (int i = 0; i < food_amount - food_counter; i++) {
			int food_pos_x = (rand() % (size_x - 2)) + 1;
			int food_pos_y = (rand() % (size_y - 2)) + 1;
			if (food_pos_x % 2 == size_x % 2) {
				if (food_pos_x >= size_x / 2) {
					food_pos_x--;
				}
				else if (food_pos_x < size_x / 2) {
					food_pos_x++;
				}
			}
			field[food_pos_y][food_pos_x] = food;
		}
	}
	void Setup_Game_Field() {
		for (int y = 0; y < size_y; y++) {
			for (int x = 0; x < size_x; x++) {
				if (field[y][x] != food) {
					if ((y == 0 || y == size_y - 1) && (x == 0 || x == size_x - 1)) {
						field[y][x] = corner;
					}
					else if (x == 0 || x == size_x - 1) {
						field[y][x] = vertical_border;
					}
					else if (y == 0 || y == size_y - 1) {
						field[y][x] = horizontal_border;
					}
					else {
						field[y][x] = inside;
					}
				}
			}
		}
		Spawn_Food();
		for (int i = 0; i < tail_length; i++) {
			field[(*(tail + i)).Get_y()][(*(tail + i)).Get_x()] = object_tail;
		}
		if (head_point.Get_x() - (*(tail)).Get_x() == step_x && step_x > 1) {
			for (int i = 1; i < step_x; i++) {
				field[(*(tail)).Get_y()][(*(tail)).Get_x() + i] = object_tail;
			}
		}
		else if ((*(tail)).Get_x() - head_point.Get_x() == step_x && step_x > 1) {
			for (int i = 1; i < step_x; i++) {
				field[head_point.Get_y()][head_point.Get_x() + i] = object_tail;
			}
		}
		for (int i = 0; i < tail_length - 1; i++) {
			if ((*(tail + i)).Get_x() - (*(tail + (i + 1))).Get_x() == step_x && step_x > 1) {
				for (int j = 1; j < step_x; j++) {
					field[(*(tail + (i + 1))).Get_y()][(*(tail + (i + 1))).Get_x() + j] = object_tail;
				}
			}
			else if ((*(tail + (i + 1))).Get_x() - (*(tail + i)).Get_x() == step_x && step_x > 1) {
				for (int j = 1; j < step_x; j++) {
					field[(*(tail + i)).Get_y()][(*(tail + i)).Get_x() + j] = object_tail;
				}
			}
		}
		field[head_point.Get_y()][head_point.Get_x()] = object_head;
		if (object_code) {
			field[(*(tail + tail_length)).Get_y()][(*(tail + tail_length)).Get_x()] = object_tail;
			if ((*(tail + tail_length)).Get_x() - (*(tail + (tail_length - 1))).Get_x() == step_x && step_x > 1) {
				for (int i = 1; i < step_x; i++) {
					field[(*(tail + (tail_length - 1))).Get_y()][(*(tail + (tail_length - 1))).Get_x() + i] = object_tail;
				}
			}
			else if ((*(tail + (tail_length - 1))).Get_x() - (*(tail + tail_length)).Get_x() == step_x && step_x > 1) {
				for (int i = 1; i < step_x; i++) {
					field[(*(tail + tail_length)).Get_y()][(*(tail + tail_length)).Get_x() + i] = object_tail;
				}
			}
		}
	}
	void Increase_Tail() {
		Point* Help_Array = new Point[tail_length + 2];
		for (int i = 0; i < tail_length + 1; i++) {
			*(Help_Array + i) = *(tail + i);
		}
		tail_length++;
		delete[] tail;
		tail = Help_Array;
	}
	void Update_Tail() {
		Point* Help_Array = new Point[tail_length + 1];
		for (int i = 1; i < tail_length + 1; i++) {
			*(Help_Array + i) = *(tail + (i - 1));
		}
		delete[] tail;
		tail = Help_Array;
	}

public:
	Snake_Engine(bool color_mode) {
		this->color_mode = color_mode;
		head_point.Set_x(size_x / 2);
		head_point.Set_y(size_y / 2);
		Setup_Game_Field();
	}
	~Snake_Engine() {
		delete[] tail;
	}

	bool Get_Object_Code_Value() {
		return object_code;
	}
	void Print_Play_Time() {
		play_timer.Print_Play_Time();
	}
	void Sleep(int value) {
		std::this_thread::sleep_for(std::chrono::milliseconds(value));
	}
	void Clear_Console() {
		COORD coord = { 0, 0 };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
		//system("cls");
		//std::cout << "\x1b[0;0H\x1b[J";
	}
	void Print_Head_Pos() {
		std::cout << std::endl << "Head Point Position: " << std::endl << "x = " << head_point.Get_x() << "\ty = " << head_point.Get_y() << std::endl;
	}
	void Print_Tail_Pos() {
		std::cout << std::endl << "Tail Positions: " << std::endl;
		for (int i = 0; i < tail_length + 1; i++) {
			std::cout << (*(tail + i)).Get_x() << "\t";
		}
		std::cout << std::endl;
		for (int i = 0; i < tail_length + 1; i++) {
			std::cout << (*(tail + i)).Get_y() << "\t";
		}
		std::cout << std::endl;
	}
	void Print_Score() {
		std::cout << "Score: " << tail_length - const_tail_length << std::endl;
	}
	void Print_Game_Field() {
		Setup_Game_Field();
		for (int y = 0; y < size_y; y++) {
			for (int x = 0; x < size_x; x++) {
				if (color_mode) {
					if (field[y][x] == vertical_border || field[y][x] == horizontal_border || field[y][x] == corner) {
						color.Set_Color(8);
					}
					else if (field[y][x] == inside) {
						color.Set_Color(0);
					}
					else if (field[y][x] == object_head) {
						color.Set_Color(2);
					}
					else if (field[y][x] == object_tail) {
						color.Set_Color(10);
					}
					else if (field[y][x] == food) {
						color.Set_Color(4);
					}
					std::cout << field[y][x];
					color.Reset_Color();
				}
				else {
					std::cout << field[y][x];
				}
			}
			std::cout << std::endl;
		}
	}
	void Object_Move() {
		field[head_point.Get_y()][head_point.Get_x()] = inside;
		Update_Tail();
		*tail = head_point;
		if (ch == 'w' || ch == 'W') {
			head_point.Set_y(head_point.Get_y() - step_y);
		}
		else if (ch == 'a' || ch == 'A') {
			head_point.Set_x(head_point.Get_x() - step_x);
		}
		else if (ch == 's' || ch == 'S') {
			head_point.Set_y(head_point.Get_y() + step_y);
		}
		else if (ch == 'd' || ch == 'D') {
			head_point.Set_x(head_point.Get_x() + step_x);
		}
		if (field[head_point.Get_y()][head_point.Get_x()] == vertical_border || field[head_point.Get_y()][head_point.Get_x()] == horizontal_border || field[head_point.Get_y()][head_point.Get_x()] == object_tail || field[head_point.Get_y()][head_point.Get_x()] == corner) {
			object_code = true;
			head_point = *(tail);
		}
		field[head_point.Get_y()][head_point.Get_x()] = object_head;
	}
 void Chech_KeyBoard_Value() {
		if (_kbhit()) {
			char last_ch = ch;
			ch = _getch();
			int correct_answer_counter = 0;
			for (int i = 0; i < 8; i++) {
				if (ch == keyboard_list[i]) {
					correct_answer_counter++;
				}
			}
			if (correct_answer_counter == 0) {
				ch = last_ch;
			}
			else {
				if ((last_ch == 'w' || last_ch == 'W') && (ch == 's' || ch == 'S')) {
					ch = last_ch;
				}
				else if ((last_ch == 'a' || last_ch == 'A') && (ch == 'd' || ch == 'D')) {
					ch = last_ch;
				}
				else if ((last_ch == 's' || last_ch == 'S') && (ch == 'w' || ch == 'W')) {
					ch = last_ch;
				}
				else if ((last_ch == 'd' || last_ch == 'D') && (ch == 'a' || ch == 'A')) {
					ch = last_ch;
				}
			}
		}
	}
    void Check_Exit_Value() {
        setlocale(LC_ALL, "ru");
        std::cout << std::endl << "Нажмите любую кнопку, чтобы закрыть это окно:";
        char ch = _getch();
    }
};

int main() {
	Snake_Engine Snake(1);
	while (true) {
		Snake.Chech_KeyBoard_Value();
		Snake.Object_Move();
		Snake.Print_Game_Field();
		Snake.Print_Score();
		Snake.Print_Play_Time();
#ifdef DEBUG
		Snake.Print_Head_Pos();
		Snake.Print_Tail_Pos();
#endif
		if (Snake.Get_Object_Code_Value()) {
			std::cout << std::endl << "Game Over!" << std::endl;
            Snake.Check_Exit_Value();
			break;
		}
		Snake.Sleep(200);
		Snake.Clear_Console();
	}
	return 0;
	//Проект был закончен 28.04.24
}