#if WIN32
#   define _MSC_VER 1
#	define _CRT_SECURE_NO_WARNINGS
#   include <windows.h>
#else
#
#endif

#include <OpenDoor.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
enum peg_colors {
	green,
	blue,
	red,
	purple,
	yellow,
	brown,
	none
};

struct highscore_t {
	char name[64];
	time_t score;
}__attribute__((packed));

struct highscore_t scores[10];

void load_highscores() {
	FILE *fptr;
	int i;
	
	for (i=0;i<10;i++) {
		sprintf(scores[i].name, "No-One");
		scores[i].score = 59 * 60 + 59;
	}
	
	fptr = fopen("scores.dat", "rb");
	if (fptr) {
		for (i=0;i<10;i++) {
			if (fread(&scores[i], sizeof(struct highscore_t), 1, fptr) == 0) {
				break;
			}
		}
		fclose(fptr);
	}
}

void update_highscores(time_t score) {
	int added = 0;
	int i;
	int j;
	for (i=0;i<10;i++) {
		if (score < scores[i].score) {
			added = 1;
			for (j=9;j>i;j--) {
				sprintf(scores[j].name, "%s", scores[j-1].name);
				scores[j].score = scores[j-1].score;
			}
			scores[i].score = score;
			snprintf(scores[i].name, 63, "%s", od_control_get()->user_name);
			break;
		}
	}
	
	if (added) {
		FILE *fptr;
		
		fptr = fopen("scores.dat", "wb");
		if (fptr) {
			for (i=0;i<10;i++) {
				fwrite(&scores[i], sizeof(struct highscore_t), 1, fptr);
			}
			fclose(fptr);
		}
	}
}

void view_highscores() {
	int i;
	od_clr_scr();
	od_send_file("hscore.ans");
	for (i=0;i<10;i++) {
		od_set_cursor(7 + i, 8);
		od_printf("%02d:%02d %s", scores[i].score / 60, scores[i].score % 60, scores[i].name);
		od_clr_line(); 
	}
	od_set_cursor(19, 23);
	od_printf("`bright white`Press any key to quit...`white`");
	od_get_key(TRUE);
}

void shuffle(int *array, size_t n)
{
	if (n > 1)
	{
		size_t i;
		for (i = 0; i < n - 1; i++)
		{
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
}
}

#if _MSC_VER
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
#else
int main(int argc, char **argv)
{
#endif

	int done = 0;
	int secret[6];
	int i, j;
	int turn = 0;
	int guess[4];
	char ch;
	int white_pegs;
	int black_pegs;
	time_t startt;
	time_t endt;
	int turndone;
	
	time_t score;
	
#if _MSC_VER
	od_parse_cmd_line(lpszCmdLine);
#else
	od_parse_cmd_line(argc, argv);
#endif
	od_init();
	srand(time(NULL));

	while (!done) {
		while (1) {
			// clear board
			od_clr_scr();
			od_send_file("mastermind.ans");

			for (i = 0; i < 6; i++) {
				secret[i] = i;
			}
			shuffle(secret, 6);
					
			for (turn = 0; turn < 12; turn++) {
				for (i = 0; i < 4; i++) {
					od_set_cursor(turn + 5, i * 2 + 9);
					od_printf("`bright black`\xfe");
				}
				for (i = 0; i < 4; i++) {
					od_set_cursor(turn + 5, i * 2 + 20);
					od_printf("  ");
				}
			}
			
			startt = time(NULL);
			
			for (turn = 0; turn < 12; turn++) {
				turndone = 0;
				for (i=0;i<4;i++) {
					guess[i] = none;
				}
				
				i = 0;
				
				while (!turndone) {
					od_set_cursor(turn + 5, i * 2 + 9);
					ch = od_get_answer("AaBbCcDdEeFfQq46\r");
					switch (tolower(ch)) {
					case 'a':
						od_printf("`bright green`\xfe`white`");
						guess[i] = green;
						break;
					case 'b':
						od_printf("`bright cyan`\xfe`white`");
						guess[i] = blue;
						break;
					case 'c':
						od_printf("`bright red`\xfe`white`");
						guess[i] = red;
						break;
					case 'd':
						od_printf("`bright magenta`\xfe`white`");
						guess[i] = purple;
						break;
					case 'e':
						od_printf("`bright yellow`\xfe`white`");
						guess[i] = yellow;
						break;
					case 'f':
						od_printf("`yellow`\xfe`white`");
						guess[i] = brown;
						break;
					case '4':
						if (i > 0) {
							i--;
						}
						break;
					case '6':
						if (i < 3) {
							i++;
						}
						break;
					case '\r':
						turndone = 1;
						for (j=0;j<4;j++) {
							if (guess[j] == none) {
								turndone = 0;
								od_set_cursor(5, 33);
								od_printf("`bright red`Please place all the pegs!`white`");
								od_clr_line();								
								break;
							}
						}
						break;
					case 'q':
						load_highscores();
						view_highscores();
					
						od_exit(0, FALSE);
					}

				}
				// calculate clue pegs.
				black_pegs = 0;
				white_pegs = 0;
				for (i = 0; i < 4; i++) {
					for (j = 0; j < 4; j++) {
						if (guess[i] == secret[j]) {
							black_pegs++;
							break;
						}
					}
				}

				for (i = 0; i < 4; i++) {
					if (guess[i] == secret[i]) {
						white_pegs++;
						black_pegs--;
					}
				}
				if (white_pegs == 4) {
					// you win
					endt = time(NULL);
					break;
				}
				// display pegs
				for (i = 0; i < 4; i++) {
					od_set_cursor(turn + 5, i * 2 + 20);
					if (white_pegs > 0) {
						od_printf("`bright white`\xfe");
						white_pegs--;
					}
					else if (black_pegs > 0) {
						od_printf("`bright black`\xfe");
						black_pegs--;
					}
				}
			}
			if (white_pegs == 4) {
				od_set_cursor(5, 33);
				score = endt - startt;
				
				od_printf("`bright green`YOU WON! `bright white`in `bright cyan`%d `bright white`minutes and `bright cyan`%d `bright white`seconds.`white`", score / 60, score % 60);
				load_highscores();
				update_highscores(score);
				od_clr_line();
			}
			for (i = 0; i < 4; i++) {
				od_set_cursor(19, i * 3 + 6);
				switch (secret[i]) {
				case green:
					od_printf("`bright green`\xdb\xdb");
					break;
				case blue:
					od_printf("`bright cyan`\xdb\xdb");
					break;
				case red:
					od_printf("`bright red`\xdb\xdb");
					break;
				case purple:
					od_printf("`bright magenta`\xdb\xdb");
					break;
				case yellow:
					od_printf("`bright yellow`\xdb\xdb");
					break;
				case brown:
					od_printf("`yellow`\xdb\xdb");
					break;
				}
			}
			od_get_key(TRUE);
		}
	}

	od_exit(0, FALSE);
}
