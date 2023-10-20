#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define FIELD_WIDTH 25
#define FIELD_LENGTH 80
#define DEFAULT_SPEED 1000000

void input_seed(int **arr);        // функция считывания 1-го поколения из stdin
void output(int **arr);            // функция отрисовки поля
void allocate_memory(int ***arr);  // функция выделения памяти для массива 25х80
int cell_check(int **field, int y_cord, int x_cord);  // функция вычисления состояния клетки на следующем шагу
void game_of_life(int **current_field, int **next_field);  // основной шаг игры
void swap_arrays(int ***first, int ***second);  // функция свопа для двумерных массивов
int is_empty(int **curr);                       // функция проверки живых клетки

int main() {
    // блок начальных настроек ncurses
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, 1);
    // создаем поля и выделяем для них память
    int **current_field, **next_field;
    allocate_memory(&current_field);
    allocate_memory(&next_field);
    // записываем шаблон в current_field
    input_seed(current_field);
    // условия конца игры: ESC, либо если все клетки мертвы
    int modifficator = 1, key = 0, generation = 0;
    while (key != 27 && !is_empty(current_field)) {
        key = getch();
        if (key == 'a' && modifficator < 1000)
            modifficator *= 5;
        else if (key == 'z' && modifficator > 1)
            modifficator /= 5;

        output(current_field);  // печатаем текущее поле
        ++generation;
        mvprintw(FIELD_WIDTH, FIELD_LENGTH / 4, "| GENERATION %.8d | SPEED: %4.d |", generation,
                 modifficator);
        game_of_life(current_field, next_field);  // вычисляем следующее поле
        swap_arrays(&current_field, &next_field);

        usleep(DEFAULT_SPEED / modifficator);  // скорость
    }

    free(current_field);
    free(next_field);

    endwin();
    return 0;
}

void input_seed(int **arr) {
    for (int **row = arr; row - arr < FIELD_WIDTH; ++row)
        for (int *col = *row; col - *row < FIELD_LENGTH; ++col) scanf("%d", col);
}

void output(int **arr) {
    for (int row = 0; row < FIELD_WIDTH; ++row) {
        for (int col = 0; col < FIELD_LENGTH; ++col)
            if (arr[row][col] == 0)
                mvaddch(row, col, ' ');
            else
                mvaddch(row, col, ACS_CKBOARD);
        if (row != FIELD_LENGTH - 1) mvaddch(row, FIELD_LENGTH - 1, '\n');
    }
}

void allocate_memory(int ***arr) {
    *arr = (int **)malloc(FIELD_WIDTH * sizeof(int *) + FIELD_LENGTH * FIELD_WIDTH * sizeof(int));
    for (int **row = *arr; row - *arr < FIELD_WIDTH; ++row)
        *row = (int *)(*arr + FIELD_WIDTH) + (row - (*arr)) * FIELD_LENGTH;
}

int cell_check(int **field, int y_cord, int x_cord) {
    int sum = 0;
    for (int y = y_cord - 1; y <= y_cord + 1; ++y)
        for (int x = x_cord - 1; x <= x_cord + 1; ++x)
            if (!(y == y_cord && x == x_cord))
                sum += field[(FIELD_WIDTH + y) % FIELD_WIDTH][(FIELD_LENGTH + x) % FIELD_LENGTH];
    return (field[y_cord][x_cord] && (sum == 2 || sum == 3)) || (!(field[y_cord][x_cord]) && sum == 3);
}

void game_of_life(int **current_field, int **next_field) {
    for (int y = 0; y < FIELD_WIDTH; ++y)
        for (int x = 0; x < FIELD_LENGTH; ++x) next_field[y][x] = cell_check(current_field, y, x);
}

void swap_arrays(int ***first, int ***second) {
    int **temp = *first;
    *first = *second;
    *second = temp;
}

int is_empty(int **curr) {
    int flag = 1;
    for (int i = 0; i < FIELD_WIDTH; i++)
        for (int j = 0; j < FIELD_LENGTH; j++) {
            if (curr[i][j] == 1) {
                flag = 0;
                break;
            }
            if (!flag) break;
        }
    return flag;
}