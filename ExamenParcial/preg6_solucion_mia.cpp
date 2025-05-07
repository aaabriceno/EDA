#include <iostream>
#include <algorithm>
using namespace std;

int computeArea(int ax1, int ay1, int ax2, int ay2,int bx1, int by1, int bx2, int by2) {
    int area1 = (ax2 - ax1) * (ay2 - ay1);
    int area2 = (bx2 - bx1) * (by2 - by1);
    
    int interseccion_A = max(0, min(ax2, bx2) - max(ax1, bx1));
    int interseccion_B = max(0, min(ay2, by2) - max(ay1, by1));
    
    int intersectadaArea = interseccion_A * interseccion_B;
    return area1 + area2 - intersectadaArea;
}

int main() {
    
    int totalArea = computeArea(-3, 3, 3, 6, 1, 1, 8, 5);
    cout << totalArea << endl; // Salida esperada : 42
    return 0;
}