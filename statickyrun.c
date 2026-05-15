
///Désolé, l'écriture de fichier ne marche pas sur caséine...

#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>

/// Alternative plus 'trans-plateforme' que 'system("clear")'
#define clear() printf("\033[H\033[J")


//// Variables Globales 
int acquisition_time ;

int max_len_name = 3;


///Fonction fournie par le professeur

// Gets a char from STDIN
// Returns immediatly, even if STDIN is empty
// in which case it returns ?
int getch() 
{ 
    int ch;
    struct termios oldattr, newattr;

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~ICANON;
    newattr.c_lflag &= ~ECHO;
    newattr.c_cc[VMIN] = 1;
    newattr.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
    
    if( poll(&mypoll, 1, acquisition_time) )
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

    return ch; 
}
/// Fin /!\

/// Struct score

struct hiscore_s {
    char* name;
    long points;
};
typedef struct hiscore_s hiscore;

/// Dessin des différents sprites & écrans titres

void draw_sprite(int row, int col, char board[row][col], int height, int pos){
    board[height+4][pos+1] = '_'; board[height+4][pos+2] = '_'; board[height+4][pos+3] = '_';
    board[height+3][pos-1] = '_'; board[height+3][pos-2] = '_'; board[height+3][pos-3] = '_'; board[height+3][pos-4] = '_'; board[height+3][pos] = '/'; board[height+3][pos+3] = '.'; board[height+3][pos+4] = '\\';
    board[height+2][pos-6] = ' '; board[height+2][pos-5] = '/'; board[height+2][pos] = '\\'; board[height+2][pos+1] = '\\'; board[height+2][pos+3] = '_'; board[height+2][pos+4] = '|'; board[height+2][pos+5] = '\\'; board[height+2][pos+6] = '_';
    board[height+1][pos-6] = '/'; board[height+1][pos-5] = '|'; board[height+1][pos-2] = '_'; board[height+1][pos+1] = '\\'; board[height+1][pos+2] = '/'; board[height+1][pos+4] = '|'; board[height+1][pos+5] = '\\'; board[height+1][pos+6] = '_';
    board[height][pos-6] = '|'; board[height][pos-5] = '|'; board[height][pos-4] = '_'; board[height][pos-3] = '|'; board[height][pos-1] = '|'; board[height][pos] = '_'; board[height][pos+1] = '|';
}

void draw_jump_sprite(int row, int col, char board[row][col], int height, int pos){
    board[height+4][pos+1] = '_'; board[height+4][pos+2] = '_'; board[height+4][pos+3] = '_';
    board[height+3][pos-1] = '_'; board[height+3][pos-2] = '_'; board[height+3][pos-3] = '_'; board[height+3][pos-4] = '_'; board[height+3][pos] = '/'; board[height+3][pos+3] = '.'; board[height+3][pos+4] = '\\';
    board[height+2][pos-6] = '|'; board[height+2][pos-5] = '/'; board[height+2][pos] = '\\'; board[height+2][pos+1] = '\\'; board[height+2][pos+3] = '_'; board[height+2][pos+4] = '|'; board[height+2][pos+5] = '\\'; board[height+2][pos+6] = '_';
    board[height+1][pos-6] = '\\'; board[height+1][pos-5] = '|'; board[height+1][pos-2] = '_'; board[height+1][pos+1] = '\\'; board[height+1][pos+2] = '/'; board[height+1][pos+4] = '|'; board[height+1][pos+5] = '\\'; board[height+1][pos+6] = '_';
    board[height][pos-5] = '/'; board[height][pos-4] = '_'; board[height][pos-3] = '/'; board[height][pos-1] = '\\'; board[height][pos] = '_'; board[height][pos+1] = '\\';
}

void clear_sprite(int row, int col, char board[row][col], int height, int pos){
    board[height+4][pos+1] = ' '; board[height+4][pos+2] = ' '; board[height+4][pos+3] = ' ';
    board[height+3][pos-1] = ' '; board[height+3][pos-2] = ' '; board[height+3][pos-3] = ' '; board[height+3][pos-4] = ' '; board[height+3][pos] = ' '; board[height+3][pos+3] = ' '; board[height+3][pos+4] = ' ';
    board[height+2][pos-6] = ' '; board[height+2][pos-5] = ' '; board[height+2][pos] = ' '; board[height+2][pos+1] = ' '; board[height+2][pos+3] = ' '; board[height+2][pos+4] = ' '; board[height+2][pos+5] = ' '; board[height+2][pos+6] = ' ';
    board[height+1][pos-6] = ' '; board[height+1][pos-5] = ' '; board[height+1][pos-2] = ' '; board[height+1][pos+1] = ' '; board[height+1][pos+2] = ' '; board[height+1][pos+4] = ' '; board[height+1][pos+5] = ' '; board[height+1][pos+6] = ' ';
    board[height][pos-6] = ' '; board[height][pos-5] = ' '; board[height][pos-4] = ' '; board[height][pos-3] = ' '; board[height][pos-1] = ' '; board[height][pos] = ' '; board[height][pos+1] = ' ';
}



void draw_title_screen(){
    printf("Bienvenue dans...\n\n");
    printf("         ___________________________________ ______  __        ___\n");
    printf("     _____/ ____/__  __/ __  /__  __/_/ ___// / /\\ \\/ / | ____/  .\\ \n");
    printf("  _______/___  /  / / / /_/ /  / / / / /__ /  <   \\  /  \\/    \\\\ _|\\_\n");
    printf("________/_____/  /_/ /_/ /_/  /_/ /_/____//_/\\_\\  /_/    |  _  \\/ |\\_\n");
    printf("                     ___________ __  ____    __          /_/ \\_\\ \n");
    printf("              ____________/ .  // / / /  \\  / / \n");
    printf("       __________________/   < / /_/ / /\\ \\/ /             l\n");
    printf("________________________/_/\\_\\/_____/_/  \\__/             l\n\n");
    printf("Contrôles : espace pour sauter. C'est tout !\n");
    printf("Entrez 'j' pour jouer, 'h' voir voir les High Scores ou 'q' pour quitter !\n");
}

void draw_game_over_screen(){
    printf("   ____________________________            ___  ___\\\\\\___  ___ \n");
    printf("  /  ___/ __  / __  __  / ____/           /   \\/         \\/   \\ \n");
    printf(" /  /  / /_/ / / /_/ / / ____/            |    |    _    |    | \n");
    printf("/_____/_/ /_/_/     /_/_____/             \\/   \\ X /-\\ X /   \\/ \n");
    printf("   ______ _    _____________               \\__/ ┌-┐|-|┌-┐ \\__/ \n");
    printf("  / __  /| \\  / / ____/ .  /                 /\\_| |\\-/| |_/\\ \n");
    printf(" / /_/ /  \\ \\/ / ____/   <  __  __  __       \\____|| ||____/ \n");
    printf("/_____/    \\__/_____/_/\\_\\ /_/ /_/ /_/             └-┘ \n\n");
}



/// Affichage du score en haut de l'écran

void draw_score(int row, int col, char board[row][col], long score){
    int n = score;
    int p = 14;
    while(n>0 && p>=9){
        board[row-1][p] = n%10+48;
        n = n/10;
        p--;
    }
}

/// Prints the game board.

void print_board(int row, int col, char board[row][col]) {
  clear();
  for (int i = row-1; i >= 0 ; i--) {
    for (int j = 0; j < col; j++)
      printf("%c", board[i][j]) ;
    printf("\n");
  }
}

/// Scrolling des obstacles

void obstacle_scrolling(int row, int col, char board[row][col], int obs_speed) {
  int pos = 4 ;
  int height = 1 ; 
  for (int i = 1; i < row; i++) {
    for (int j = 0; j < col; j++)
      if (board[i][j] == 'l') {
	board[i][j] = ' ' ;
	if (j >= obs_speed)
	  board[i][j-obs_speed] = 'l' ;
      }
  }
}

/// Initialize the board.

void init_board(int row, int col, char board[row][col]) {
  for (int j = 0; j < col; j++) {
    board[0][j] = 'M' ;
  }
  for (int i = 1; i < row; i++) {
    for (int j = 0; j < col; j++)
      board[i][j] = ' ' ;
  }
  board[row-1][0] = 'S'; board[row-1][1] = 'c'; board[row-1][2] = 'o'; board[row-1][3] = 'r'; board[row-1][4] = 'e'; board[row-1][6] = ':';
  int height = 1;
  int pos = 7;
  draw_sprite(row, col, board, height, pos);
}

/// Ajout du score au fichier

hiscore* read_scores(char* filename, int nb_high_scores){
    hiscore* scores = malloc(sizeof(hiscore)*nb_high_scores);
        for(int i=0; i<nb_high_scores; i++){
            scores[i].name = malloc(sizeof(char)*max_len_name);
            scores[i].points = 0;
        }
    FILE* ptr = fopen(filename,"r");
    if (NULL == ptr) {
        printf("file can't be opened, creating one \n");
        ptr = fopen(filename, "w");
    } else {
        long v;
        char* name = malloc(sizeof(char)*max_len_name);
        int i = 0; 
        int stop = fscanf(ptr, "%s %ld\n", name, &v);
        while (stop > 0 && i<nb_high_scores) {
            scores[i].points = v;
            strcpy(scores[i].name,name); 
            stop = fscanf(ptr, "%s %ld\n", name, &v);
            i++;
        }
    }
    fclose(ptr);
    return scores;
}


//marche pas sur caséine :/
void write_scores(char* filename, int nb_high_scores, hiscore* scores){
    FILE* ptr2 = fopen(filename, "w");
    for(int i=0; i<nb_high_scores; i++){
        if(scores[i].points>0){
            fprintf(ptr2, "%s %ld\n", scores[i].name, scores[i].points);
        }
    }
    fclose(ptr2);
}

void add_score(hiscore* scores, int nb_high_scores, long score){
    if(score < scores[nb_high_scores-1].points){
        return; //Ce n'est pas un high score, on ne fait rien
    }
    char* input = malloc(sizeof(char)*max_len_name);
    char* name = malloc(sizeof(char)*max_len_name);
    printf("High score ! Entrez votre pseudo (3 lettres) :\n");
    scanf("%s", input);
    //printf("input : %s\n", input);
    int i =0;
    while(input[i]!='\0'){
        if(i<max_len_name){
            name[i] = input[i];
        }
        i++;
    }
    bool end_input = false;
    for(int i=0; i<max_len_name; i++){
        if(input[i]=='\0'){
            end_input = true;
        }
        if(end_input){
            name[i] = '_';
        } else {
            name[i] = input [i];
        }
    }
    //On insère le score que l'on vient de faire dans le tableau de sorte à ce qu'il reste trié
    for(int i=0; i<nb_high_scores; i++){
        if(score >  scores[i].points){
            for(int j=nb_high_scores-1; j>i; j--){
                //les deux chaînes doivent faire la même longueur
                strcpy(scores[j].name, scores[j-1].name);
                scores[j].points = scores[j-1].points;
            }
            scores[i].name = name;
            scores[i].points = score;
            break;
        }
    }
}

///Affichage des High Scores

void display_high_scores(hiscore* scores, int nb_high_scores){
    printf("\nHigh Scores : \n\n");
    for(int i=0; i<nb_high_scores; i++){
        if(scores[i].points>0){
            printf("%d. %s : %ld\n", i+1, scores[i].name, scores[i].points);
        }
    }
    printf("\n");
}


/// Main

int main () {
    srand(time(NULL));

    //Refresh Rate
    double refresh_rate = 0.05;

    // Init Board
    char board[26][100] ;
    int row = 26  ;
    int col = 100 ;

    //Position du personnage
    int pos = 7 ;
    int height = 1 ;

    //Paramètres et variables du saut 
    int jump_height = height + 10;
    int prev_jump_height = height;

    //Paramètres et variables des obstacles
    int pos_obs = col-1;
    int obs_speed = 7;

    //Booléens pour les sauts & obstacles
    bool is_jumping = false;
    bool obstacle_on_screen = false;

    //Booléens pour les boucles de jeu & de menu
    bool in_menu = true;
    bool in_game = true;
    bool playing = false;
    bool in_game_over  = false;

    //Probabilité d'apparition des obstacles
    int spawn_prob = 11;
    int spawn_obs = rand() % spawn_prob;

    //Variables et paramètres des scores & high scores
    long score = 0;
    int nb_high_scores = 100;
    char* filename = "scores.txt";

    // Time variables
    struct timeval last, now;

    //Initialisation du board et du tableau des scores
    clear();

    char c;
    while(in_game){
        //Initialisation du board et du tableau des scores
        init_board(row, col, board);

        hiscore* scores = malloc(sizeof(hiscore)*nb_high_scores);
        scores = read_scores(filename, nb_high_scores);

        clear();

        //Affichage écran titre
        if(in_menu){
            draw_title_screen();
        }
        //Boucle écran titre
        while(in_menu){
            scanf("%c",&c);
            if(c == 'j'){
                //Réinitialisation des variables de jeu
                pos = 7 ;
                height = 1 ;
                jump_height = height + 10;
                prev_jump_height = height;
                pos_obs = col-1;
                obs_speed = 7;
                obstacle_on_screen = false;
                is_jumping = false;
                score = 0;

                in_menu = false;
                playing = true;
            } else if(c=='h') {
                clear();
                display_high_scores(scores, nb_high_scores);
                printf("\nEntrez 'j' pour jouer ou 'q pour quitter !\n");
            } else if (c=='q'){
                in_menu = false;
                in_game = false;
            } else if(c!='\n') {
                printf("Input non reconnue\n");
            }
        }

        // Game Loop
        print_board(row, col, board) ;
        int n = 0 ;
    
        while (playing) {
            int n  = getch ();
            if (spawn_obs == spawn_prob-1 && !obstacle_on_screen){
                board[1][col-1] = 'l';
                board[2][col-1] = 'l';
                board[3][col-1] = 'l';
            obstacle_on_screen = true;
            pos_obs = col-1;
            }
            if(n==' ' && !is_jumping){
                is_jumping = true;
            }


            //Refresh
            gettimeofday(&now, NULL);
            double time_taken;

            time_taken = (now.tv_sec - last.tv_sec) + (now.tv_usec - last.tv_usec) * 1e-6;

            if (time_taken > refresh_rate) {

                spawn_obs = rand() % spawn_prob;

                score++;
                obs_speed = score / 200 + 3;

                obstacle_scrolling(row, col, board, obs_speed);
                print_board(row, col, board) ;
                draw_score(row, col, board, score);

                if(obstacle_on_screen){
                    pos_obs = pos_obs - obs_speed;
                    //Détection de collision avec le joueur
                    if (pos_obs <= pos+4 && (!is_jumping || jump_height <= height + 3)){
                        playing = false;
                        in_game_over = true;
                        break;
                    }
                    //Détection du moment où l'obstacle arrive au bord, pour pouvoir en faire apparaître un autre
                    if(pos_obs <= 0){
                        obstacle_on_screen = false;
                        pos_obs = col-1;
                    }
                }

                if(is_jumping){
                    clear_sprite(row, col, board, prev_jump_height, pos);
                    draw_jump_sprite(row, col, board, jump_height, pos);
                    prev_jump_height = jump_height;
                    jump_height--;
                    if(jump_height==height-1){
                        is_jumping = false;
                        jump_height = height + 10;
                        prev_jump_height = height;
                    }
                } else {
                    draw_sprite(row, col, board, height, pos);
                }

                last = now ;
            }
        }
        //Affichage du Game Over et des High Scores
        clear();
        if(in_game_over){
            draw_game_over_screen();
            printf("Score : %ld.\n\n", score);  
            add_score(scores, nb_high_scores, score);
            write_scores(filename, nb_high_scores, scores);
            display_high_scores(scores, nb_high_scores);
            printf("Entrez 'm' pour revenir au menu ou sur 'q' pour quitter.\n");
        }

        //Boucle Game Over
        while(in_game_over){
            scanf("%c",&c);
            if(c == 'm'){
                in_game_over = false;
                clear();
                in_menu = true;
            } else if (c=='q'){
                in_game_over = false;
                in_game = false;
            } else if(c!='\n') {
                printf("Input non reconnue\n");
            }
        }
    }
}