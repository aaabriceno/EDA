#include "octree.hpp"
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <limits>
#include <algorithm>
#include <sstream>
using namespace std;

//Funciones extra
int determinar_octante(Point bottomleft, double h, Point p){
    Point centro(bottomleft.x + h/2,bottomleft.y + h/2, bottomleft.z + h/2);
    int x_bit = (p.x >= centro.x) ? 1 : 0;
    int y_bit = (p.y >= centro.y) ? 1 : 0;
    int z_bit = (p.z >= centro.z) ? 1 : 0;
    int octante = (x_bit << 2) | (y_bit << 1) | z_bit;
    return octante;
}

//Contructor
Octree::Octree(const Point &p, double height, int capacity){
    bottomLeft = p;
    h = height;
    nPoints = capacity;

    for (int i = 0; i < 8; i++){children[i] = nullptr;}
    Point(50, 60, 70);
};

//Funciones dadas
bool Octree::exist(const Point& p) {
    if (esHoja()){
        for (const auto& pt : puntos) {
            if (pt.x == p.x && pt.y == p.y && pt.z == p.z)
                return true;
        }
        return false;
    }
    int octante = determinar_octante(bottomLeft, h, p);
    if (children[octante] != nullptr) {
        return children[octante]->exist(p);
    }
    return false;
}

void Octree::insert(const Point& p){
    if (exist(p)) return;
    
    if (esHoja()) {
        if (puntos.size() < nPoints) {
            puntos.push_back(p);
            return;
        } else {
            subdividir();
            for (const auto& point : puntos) {
                int octante = determinar_octante(bottomLeft, h, point);
                children[octante]->insert(point);
            }
            puntos.clear();
        }
    }
    
    int octante = determinar_octante(bottomLeft, h, p);
    if (children[octante] ==nullptr){
        double h1 = h / 2;
        int x_offset = (octante & 4) ? h1 : 0;
        int y_offset = (octante & 2) ? h1 : 0;
        int z_offset = (octante & 1) ? h1 : 0;
        Point newBottomLeft(bottomLeft.x + x_offset, bottomLeft.y + y_offset, bottomLeft.z + z_offset);
        children[octante] = new Octree(newBottomLeft, h1, nPoints);
    }
    children[octante]->insert(p);
}

//Point Octree::find_closest(const Point&, int radius){}

//Funciones creadas
bool Octree::esHoja(){
    for (int i = 0; i < 8; i++){
        if (children[i] != nullptr){
            return false;
        }
    }
    return true;
}

void Octree::subdividir(){
    double h1 = h / 2;
    for (int i = 0; i < 8; i++){
        int x_offset = (i & 4) ? h1 : 0;
        int y_offset = (i & 2) ? h1 : 0;
        int z_offset = (i & 1) ? h1 : 0;
        Point newBottomLeft(bottomLeft.x + x_offset, bottomLeft.y + y_offset, bottomLeft.z + z_offset);
        children[i] = new Octree(newBottomLeft, h1, nPoints);
    }
}

void Octree::nueva_cant_nPoint (int newnPoint){
    nPoints = newnPoint;
    if (!esHoja()){
        for (int i = 0; i < 8; i++){
            if (children[i] != nullptr){
                children[i] -> nueva_cant_nPoint(newnPoint);
            }
        }
    }
}

vector<Point> leer_csv(const string& filename) {
    vector<Point> puntos;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        double x, y, z;
        char comma;
        if (ss >> x >> comma >> y >> comma >> z) {
            puntos.push_back(Point(x, y, z));
        }
    }
    return puntos;
}

void Octree::imprimir(int nivel) {
    for (int i = 0; i < nivel; i++) cout << "  ";
    cout << "Nodo en (" << bottomLeft.x << ", " << bottomLeft.y << ", " << bottomLeft.z << ") con h = " << h << "\n";
    for (const auto& p : puntos) {
        for (int i = 0; i < nivel + 1; i++) cout << "  ";
        cout << "Punto: (" << p.x << ", " << p.y << ", " << p.z << ")\n";
    }
    for (int i = 0; i < 8; i++) {
        if (children[i] != nullptr) {
            children[i]->imprimir(nivel + 1);
        }
    }
}

int main(){
    vector<Point> puntos = leer_csv("points1.csv");
    
    if (puntos.empty()) {
        cout << "No se encontraron puntos en el archivo CSV." << endl;
        return 1;
    }
    
    int x_min = numeric_limits<int>::max(), x_max = numeric_limits<int>::min();
    int y_min = numeric_limits<int>::max(), y_max = numeric_limits<int>::min();
    int z_min = numeric_limits<int>::max(), z_max = numeric_limits<int>::min();

    for (const auto& p : puntos){
        x_min = min(x_min, p.x);
        x_max = max(x_max, p.x);
        y_min = min(y_min, p.y);
        y_max = max(y_max, p.y);
        z_min = min(z_min, p.z);
        z_max = max(z_max, p.z);
    }

    Point bottomLeft(x_min, y_min, z_min);
    double h = max({x_max - x_min, y_max - y_min, z_max - z_min});
    Octree miOctree(bottomLeft, h, 5);

    for (const auto& p : puntos) {
        miOctree.insert(p);
    }
    
    miOctree.imprimir(0);

    cout << "BottomLeft: (" << miOctree.getbottomleft().x << ", " << miOctree.getbottomleft().y << ", " << miOctree.getbottomleft().z << ")\n";
    cout << "Altura " << miOctree.getH() << "\n";

    

    return 0;
}

