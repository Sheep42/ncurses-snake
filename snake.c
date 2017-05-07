#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#define DELAY 30000
#define TIMEOUT 10 

/* Global Vars */
	typedef enum {
		LEFT,
		RIGHT,
		UP,
		DOWN
	} direction_type;

	typedef struct point {
		int x;
		int y;
	} point;

	int x = 0,
		y = 0,
		maxY = 0, 
		maxX = 0,
		nextX = 0,
		nextY = 0,
		tailLength = 5;

	bool gameOver = false;

	direction_type currentDir = RIGHT;
	point snakeParts[255] = {};
	point food;

/* Functions */
	void createFood() {
		//Food.x is a random int between 10 and maxX - 10
		food.x = (rand() % (maxX - 20)) + 10;

		//Food.y is a random int between 5 and maxY - 5
		food.y = (rand() % (maxY - 10)) + 5;
	}
	
	void drawPart(point drawPoint) {
		mvprintw(drawPoint.y, drawPoint.x, "o");
	}

	void cursesInit() {
		initscr(); //Initialize the window
		noecho(); //Don't echo keypresses
		cbreak();
		timeout(TIMEOUT);
		curs_set(FALSE); //Don't display a cursor
	}

	void init() {
		srand(time(NULL));

		currentDir = RIGHT;
		tailLength = 5;
		gameOver = false;

		clear(); //Clears the screen
		
		//Set the initial snake coords 
		int j = 0;
		for(int i = tailLength; i >= 0; i--) {
			point currPoint;
			
			currPoint.x = i;
			currPoint.y = 0;

			snakeParts[j] = currPoint;
			j++;
		}

		//Global var stdscr is created by the call to initscr()
		getmaxyx(stdscr, maxY, maxX);

		createFood();

		refresh();
	}

	void shiftSnake() {
		point tmp = snakeParts[tailLength - 1];

		for(int i = tailLength - 1; i > 0; i--) {
			snakeParts[i] = snakeParts[i-1];
		}

		snakeParts[0] = tmp;
	}

/* Main */
	int main(int argc, char *argv[]) {
		cursesInit();
		init();

		int ch;
		while(1) {
			//Global var stdscr is created by the call to initscr()
			//This tells us the max size of the terminal window at any given moment
			getmaxyx(stdscr, maxY, maxX);
			
			if(gameOver) {
				sleep(2);
				init();
			}

			/* Input Handler */
				ch = getch();

				if(( ch=='l' || ch=='L' ) && (currentDir != RIGHT && currentDir != LEFT)) {
					currentDir = RIGHT;
				} else if (( ch=='h' || ch=='H' ) && (currentDir != RIGHT && currentDir != LEFT)) {
					currentDir = LEFT;
				} else if((ch=='j' || ch=='J') && (currentDir != UP && currentDir != DOWN)) {
					currentDir = DOWN;
				} else if((ch=='k' || ch=='K') && (currentDir != UP && currentDir != DOWN)) {
					currentDir = UP;
				}

			/* Movement */
				nextX = snakeParts[0].x;
				nextY = snakeParts[0].y;

				if(currentDir == RIGHT) nextX++;
		    	else if(currentDir == LEFT) nextX--;
				else if(currentDir == UP) nextY--;
				else if(currentDir == DOWN) nextY++;

				if(nextX == food.x && nextY == food.y) {
					point tail;
					tail.x = nextX;
					tail.y = nextY;

					snakeParts[tailLength] = tail;

					tailLength++;

					createFood();
				} else {
					//We are going to set the tail as the new head
					snakeParts[tailLength - 1].x = nextX;
					snakeParts[tailLength - 1].y = nextY;
				}
				//Shift all the snake parts
				shiftSnake();

			//Clears the screen
			clear(); 

			//Game Over if the player hits the screen edges
			if((nextX >= maxX || nextX < 0) || (nextY >= maxY || nextY < 0)) {
				mvprintw(maxY / 2, maxX / 2, "Game Over!");
				gameOver = true;
			}

			//Draw the snake to the screen
			for(int i = 0; i < tailLength; i++) {
				drawPart(snakeParts[i]);
			}

			//Draw the current food
			drawPart(food);

			//ncurses refresh
			refresh();

			//Delay between movements
			usleep(DELAY); 
		}

		endwin(); //Restore normal terminal behavior
		nocbreak();

		return 0;
	}
