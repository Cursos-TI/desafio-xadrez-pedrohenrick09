#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

char board[8][8];
int currentPlayer = 0; // 0 for White, 1 for Black

void initialize_board() {
    char initial_board[8][8] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };
    memcpy(board, initial_board, sizeof(char) * 64);
}

void print_board() {
    printf("\n  +------------------------+\n");
    for (int i = 0; i < 8; i++) {
        printf("%d |", 8 - i);
        for (int j = 0; j < 8; j++) {
            printf(" %c ", board[i][j]);
        }
        printf("|\n");
    }
    printf("  +------------------------+\n");
    printf("    a  b  c  d  e  f  g  h\n");
}

int is_valid_square(int r, int c) {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

int is_opponent_piece(int r, int c, int player) {
    if (!is_valid_square(r, c) || board[r][c] == ' ') return 0;
    return isupper(board[r][c]) ? (player == 1) : (player == 0);
}

int is_player_piece(int r, int c, int player) {
    if (!is_valid_square(r, c) || board[r][c] == ' ') return 0;
    return isupper(board[r][c]) ? (player == 0) : (player == 1);
}

int is_in_check(int player) {
    int king_r, king_c;
    char king_char = (player == 0) ? 'K' : 'k';
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (board[r][c] == king_char) {
                king_r = r;
                king_c = c;
                break;
            }
        }
    }

    int opponent = 1 - player;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (is_player_piece(r, c, opponent)) {
                char piece = board[r][c];
                int dr, dc;
                switch (tolower(piece)) {
                    case 'p':
                        dr = (opponent == 0) ? 1 : -1;
                        if (is_valid_square(r + dr, c - 1) && r + dr == king_r && c - 1 == king_c) return 1;
                        if (is_valid_square(r + dr, c + 1) && r + dr == king_r && c + 1 == king_c) return 1;
                        break;
                    case 'n':
                        for (int i = -2; i <= 2; i++) {
                           for (int j = -2; j <= 2; j++) {
                               if (abs(i) + abs(j) == 3) {
                                   if (r + i == king_r && c + j == king_c) return 1;
                               }
                           }
                        }
                        break;
                    case 'r':
                    case 'q':
                        for(dr = -1; dr <= 1; dr++) for(dc = -1; dc <= 1; dc++){
                            if(abs(dr) + abs(dc) != 1 && tolower(piece) == 'r') continue;
                            if(abs(dr) + abs(dc) == 0) continue;
                            for(int i=1; i<8; i++){
                                int nr = r + i*dr, nc = c + i*dc;
                                if(!is_valid_square(nr,nc)) break;
                                if(nr == king_r && nc == king_c) return 1;
                                if(board[nr][nc] != ' ') break;
                            }
                        }
                        if(tolower(piece) == 'r') break;
                    case 'b':
                        for(dr = -1; dr <= 1; dr+=2) for(dc = -1; dc <= 1; dc+=2){
                             for(int i=1; i<8; i++){
                                int nr = r + i*dr, nc = c + i*dc;
                                if(!is_valid_square(nr,nc)) break;
                                if(nr == king_r && nc == king_c) return 1;
                                if(board[nr][nc] != ' ') break;
                            }
                        }
                        break;
                    case 'k':
                        for (dr = -1; dr <= 1; dr++) for (dc = -1; dc <= 1; dc++) {
                            if (dr == 0 && dc == 0) continue;
                            if (r + dr == king_r && c + dc == king_c) return 1;
                        }
                        break;
                }
            }
        }
    }
    return 0;
}

int is_move_legal(int r1, int c1, int r2, int c2, int player) {
    char piece = board[r1][c1];
    if (!is_player_piece(r1, c1, player) || is_player_piece(r2, c2, player)) return 0;
    
    int dr = r2 - r1, dc = c2 - c1;

    switch (tolower(piece)) {
        case 'p': {
            int dir = (player == 0) ? -1 : 1;
            int start_row = (player == 0) ? 6 : 1;
            if (dc == 0 && board[r2][c2] == ' ') { // Move forward
                if (dr == dir) return 1;
                if (dr == 2 * dir && r1 == start_row && board[r1 + dir][c1] == ' ') return 1;
            } else if (abs(dc) == 1 && dr == dir && is_opponent_piece(r2, c2, player)) { // Capture
                return 1;
            }
            return 0;
        }
        case 'r':
            if (dr != 0 && dc != 0) return 0;
            break;
        case 'b':
            if (abs(dr) != abs(dc)) return 0;
            break;
        case 'q':
            if (abs(dr) != abs(dc) && dr != 0 && dc != 0) return 0;
            break;
        case 'n':
            return abs(dr * dc) == 2;
        case 'k':
             if (abs(dr) <= 1 && abs(dc) <= 1) return 1;
             if (dr == 0 && abs(dc) == 2 && !is_in_check(player)) { // Castling
                int row = (player == 0) ? 7 : 0;
                int rook_col = (dc > 0) ? 7 : 0;
                char rook = board[row][rook_col];
                if (r1 != row || c1 != 4 || tolower(rook) != 'r') return 0;
                int step = (dc > 0) ? 1 : -1;
                for (int c = c1 + step; c != rook_col; c += step) {
                    if (board[r1][c] != ' ') return 0;
                }
                char temp_board[8][8]; memcpy(temp_board, board, 64);
                board[r1][c1+step] = board[r1][c1]; board[r1][c1] = ' ';
                if(is_in_check(player)) { memcpy(board, temp_board, 64); return 0;}
                memcpy(board, temp_board, 64);
                return 1;
             }
             return 0;
    }
    
    // Path check for Rook, Bishop, Queen
    if(tolower(piece) == 'r' || tolower(piece) == 'b' || tolower(piece) == 'q'){
        int step_r = (dr == 0) ? 0 : (dr > 0 ? 1 : -1);
        int step_c = (dc == 0) ? 0 : (dc > 0 ? 1 : -1);
        int r = r1 + step_r, c = c1 + step_c;
        while (r != r2 || c != c2) {
            if (board[r][c] != ' ') return 0;
            r += step_r; c += step_c;
        }
    }
    return 1;
}

int has_legal_move(int player) {
    char temp_board[8][8];
    for (int r1 = 0; r1 < 8; r1++) {
        for (int c1 = 0; c1 < 8; c1++) {
            if (is_player_piece(r1, c1, player)) {
                for (int r2 = 0; r2 < 8; r2++) {
                    for (int c2 = 0; c2 < 8; c2++) {
                        if (is_move_legal(r1, c1, r2, c2, player)) {
                            memcpy(temp_board, board, 64);
                            char captured = board[r2][c2];
                            board[r2][c2] = board[r1][c1];
                            board[r1][c1] = ' ';
                            if (!is_in_check(player)) {
                                memcpy(board, temp_board, 64);
                                return 1;
                            }
                            memcpy(board, temp_board, 64);
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int main() {
    char move_str[5];
    initialize_board();

    while (1) {
        print_board();
        const char* player_name = (currentPlayer == 0) ? "Branco" : "Preto";
        printf("\nJogador: %s\n", player_name);

        if (is_in_check(currentPlayer)) {
            if (!has_legal_move(currentPlayer)) {
                printf("XEQUE-MATE! %s venceu!\n", (currentPlayer == 0) ? "Preto" : "Branco");
                break;
            }
            printf("Voce esta em XEQUE!\n");
        } else {
            if (!has_legal_move(currentPlayer)) {
                printf("EMPATE por afogamento (Stalemate)!\n");
                break;
            }
        }
        
        printf("Digite seu movimento (ex: e2e4): ");
        scanf("%s", move_str);

        if (strlen(move_str) != 4) {
            printf("Formato invalido. Tente novamente.\n");
            continue;
        }

        int c1 = tolower(move_str[0]) - 'a';
        int r1 = '8' - move_str[1];
        int c2 = tolower(move_str[2]) - 'a';
        int r2 = '8' - move_str[3];

        if (!is_valid_square(r1, c1) || !is_valid_square(r2, c2) || !is_move_legal(r1, c1, r2, c2, currentPlayer)) {
            printf("Movimento ilegal. Tente novamente.\n");
            continue;
        }

        char temp_board[8][8];
        memcpy(temp_board, board, 64);
        char piece = board[r1][c1];

        if (tolower(piece) == 'k' && abs(c2-c1) == 2) { // Handle castling move
             board[r2][c2] = board[r1][c1];
             board[r1][c1] = ' ';
             int rook_c1 = (c2 > c1) ? 7 : 0;
             int rook_c2 = (c2 > c1) ? c2-1 : c2+1;
             board[r1][rook_c2] = board[r1][rook_c1];
             board[r1][rook_c1] = ' ';
        } else {
             board[r2][c2] = board[r1][c1];
             board[r1][c1] = ' ';
        }

        if (is_in_check(currentPlayer)) {
            printf("Movimento ilegal, seu rei esta ou ficara em xeque.\n");
            memcpy(board, temp_board, 64);
            continue;
        }
        
        if (tolower(piece) == 'p' && (r2 == 0 || r2 == 7)) {
            board[r2][c2] = (currentPlayer == 0) ? 'Q' : 'q';
            printf("Peao promovido a Rainha!\n");
        }
        
        currentPlayer = 1 - currentPlayer;
    }

    return 0;
}