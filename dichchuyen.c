#include <iostream>
#include <vector>
#include <map>
#include <conio.h> // Thư viện giúp đọc phím bấm trực tiếp (không cần Enter)

using namespace std;

vector<string> gameMap = {
    "###############",
    "#P....#......A#",
    "#.###.#.###...#",
    "#.....#...#...#",
    "#.#####.#.#...#",
    "#...B...#.....#",
    "###############"
};

int main() {
    int rows = gameMap.size(), cols = gameMap[0].size();
    int playerX, playerY;
    map<char, pair<int,int>> portal;

    // 1. Tìm vị trí ban đầu của P và các Portal
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            char c = gameMap[i][j];
            if(c == 'P') {
                playerX = i;
                playerY = j;
                gameMap[i][j] = '.'; // Xóa 'P' khỏi khung cảnh tĩnh
            }
            if(c >= 'A' && c <= 'Z') {
                portal[c] = {i, j};
            }
        }
    }

    while(true) {
        // Clear màn hình cũ để vẽ bản đồ mới tại cùng một vị trí
        system("cls");

        // 2. In bản đồ
        cout << "=== GAME MOVE & PORTAL ===\n";
        cout << "Dung cac phim W-A-S-D de di chuyen (Nhan Q de thoat)\n\n";

        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++) {
                if(i == playerX && j == playerY)
                    cout << 'P';
                else
                    cout << gameMap[i][j];
            }
            cout << "\n";
        }

        // 3. ĐỌC PHÍM TRỰC TIẾP (Không cần bấm Enter)
        char move = getch();

        if(move == 'Q' || move == 'q') break;

        int nx = playerX, ny = playerY;

        // Xử lý di chuyển (chấp nhận cả chữ hoa lẫn chữ thường)
        if(move == 'W' || move == 'w') nx--;
        else if(move == 'S' || move == 's') nx++;
        else if(move == 'A' || move == 'a') ny--;
        else if(move == 'D' || move == 'd') ny++;
        else continue; // Bấm phím khác WASD thì bỏ qua

        // 4. Kiểm tra va chạm Tường (#) và Mép bản đồ
        if(nx < 0 || nx >= rows || ny < 0 || ny >= cols || gameMap[nx][ny] == '#')
            continue;

        // Cập nhật vị trí mới
        playerX = nx;
        playerY = ny;

        // 5. Xử lý Portal A <-> B
        char cell = gameMap[playerX][playerY];
        if(cell >= 'A' && cell <= 'Z') {
            char other = (cell % 2 == 1) ? cell + 1 : cell - 1;
            if(portal.count(other)) {
                playerX = portal[other].first;
                playerY = portal[other].second;
            }
        }
    }

    system("cls");
    cout << "Da thoat game!\n";
    return 0;
}
