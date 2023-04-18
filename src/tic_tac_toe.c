#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../include/dicts.h"

struct TTT {
    int k;
    int n;
    int* grid;
};

typedef struct TTT ttt;

ttt* game_init(int n,int k){
    ttt* game = malloc(sizeof(ttt));
    game->k = k;
    game->n = n;
    game->grid = malloc(sizeof(int) * n * n);
    for (int i = 0; i < n * n;i++){
        game->grid[i] = 0;
    }
    return game;
}

void game_free(ttt* game){
    free(game->grid);
    free(game);
}

int* histogram(ttt* game){
    int* hist = malloc(3 * sizeof(*hist));
    for (int i = 0; i < 3; i++) hist[i] = 0;
    for (int i = 0; i < game->n * game->n; i++){
        int val = game->grid[i];
        assert (0 <= val && val < 3);
        hist[val] += 1;
    }
    return hist;
}

/*
Player 1 plays first.
So if both players played the same number of times, 
it is now p1's turn.
*/
int current_player(ttt* game){
    int* hist = histogram(game);
    int res;
    if (hist[0] == 0) res = 0;
    else if (hist[1] == hist[2]) res = 1;
    else res = 2;
    free(hist);
    return res;
}

void play(ttt* game, int line, int column) {
    int i = line * game->n + column;
    if (game->grid[i] != 0) {
        printf("Something is already placed here. Pick another case");
    } else {
        game->grid[i] = current_player(game);
    }
}

bool winning_lining_up(ttt* game,int i,int di,int player){
    int k = game->k;
    int n = game->n;

    int column = i % n;
    int line = i / n;

    int dc = ((di + 1) % n) - 1;
    int dl = (di + 1) / n;
    for (int j = 0; j < k; j++){
        if (line < 0 || line >= n || column < 0 || column >= n) return false;
        i = line * n + column;
        if (game->grid[i] != player) return false;
        column += dc;
        line += dl;
    }
    return true;
}

bool winner(ttt* game,int player){
    int n = game->n;
    int arr_di[4] = {1, n - 1, n, n + 1};
    for (int i = 0; i < n * n; i++){
        for (int j = 0; j < 4; j++){
            if (winning_lining_up(game,i,arr_di[j],player)) return true;
        }
    }
    return false;
}


/*
Every game can be encoded as an integer with n² digits in base 3. Efficient storage.
*/
int encoding(ttt* game){
    int key = 0;
    int n = game->n;
    for (int i = 0; i < n * n; i++){
        key = 3 * key + game->grid[i];
    }
    return key;
}

/*
3 - player is the other player
*/
int attracts(ttt* game, dict* d){
    int key = encoding(game);
    int n = game->n;
    int player = current_player(game);
    if (!member(d,key)) {
        if (winner(game,player) ) add(d,key,player);
        else if (winner(game,3 - player)) add(d,key,3 - player);
        else if (player == 0) add(d,key,0);
        else {
            int tab_attracts[3] = {0,0,0};
            for (int i = 0; i < n * n; i++){
                if (game->grid[i] == 0) {
                    game->grid[i] = player;
                    int att = attracts(game,d);
                    game->grid[i] = 0;
                    tab_attracts[att] += 1;
                    if (att == player) break;
                }
            }
            if (tab_attracts[player] != 0) add(d,key,player);
            else if (tab_attracts[0] != 0) add(d,key,0);
            else add(d,key,3 - player);
        }
    }
    return get(d,key);
}

int optimal_strat(ttt* game,dict* d){
    int att = attracts(game,d);
    int n = game->n;
    int player = current_player(game);
    for (int i = 0; i < n * n; i++) {
        if (game->grid[i] == 0) {
            game->grid[i] = player;
            int att2 = attracts(game, d);
            game->grid[i] = 0;
            if (att == att2) return i;
        }
    }
}


void display_line_sep(int n){
    printf("\n ");
    for (int i = 0; i < n; i++){
        printf("+-");
    }
    printf("+\n");
}

void display(ttt* jeu){
    int n = jeu->n;
    char arr[3] = {' ', 'X', 'O'};
    printf(" ");
    for (int cln = 0; cln < n; cln++){
        printf(" %d", cln);
    }
    display_line_sep(n);
    for (int lgn = 0; lgn < n; lgn++){
        printf("%d|", lgn);
        for (int cln = 0; cln < n; cln++){
            int i = n * lgn + cln;
            printf("%c|", arr[jeu->grid[i]]);
        }
        display_line_sep(n);
    }
    printf("\n");
}

void game_play(int k, int n){
    ttt* game = game_init(k,n);
    char c;
    int bot ,line,column;
    while (true) {
        printf("Would you like to play first (y/n)\n");
        scanf("%c",&c);
        if (c == 'y'){
            bot = 2;
            break;
        } else if (c == 'n') {
            bot = 1;
            break;
        }
    }
    dict* d = create();
    int player = 1;
    while (player != 0 && !winner(game,1) && !winner(game,2)){
        display(game);
        if (player == bot){
            int i = optimal_strat(game,d);
            column = i % n;
            line = i / n;
            printf("Bot plays line %d and columns %d\n",line,column);
            play(game,line,column);
        } else {
            printf("Your turn to play\n Type line column :\n");
            scanf("%d %d",&line,&column);
            if (column  < 0 || column >= game->n || line < 0 || line >= game->n){
                printf("Out of bounds");
            } else {
                play(game,line,column);
            }
        }
        player = current_player(game);
    }
    display(game);
    if (winner(game,bot)) printf("The bot won.");
    else if (winner(game,3 - bot)) printf("You actually won.");
    else printf("Even, quite surprising.");
    game_free(game);
    dict_free(d);
}

int main(void){
    game_play(4,4);
    return 0;
}