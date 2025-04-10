#include "octree.hpp"
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <limits>
#include <algorithm>
#include <sstream>
#include <functional>
using namespace std;

//Funciones extra
//Funcion que determina el octante en el que debe estar un punto
int determinar_octante(Point bottomleft, double h, Point p) {
    Point centro(bottomleft.x + h / 2, bottomleft.y + h / 2, bottomleft.z + h / 2);
    int x_bit = (p.x >= centro.x) ? 1 : 0;
    int y_bit = (p.y >= centro.y) ? 1 : 0;
    int z_bit = (p.z >= centro.z) ? 1 : 0;
    int octante = (x_bit << 2) | (y_bit << 1) | z_bit;
    return octante;
}

//Funciones dadas
//Contructor
Octree::Octree(const Point& p, double height, int capacity) {
    bottomLeft = p;
    h = height;
    nPoints = capacity;

    for (int i = 0; i < 8; i++) { children[i] = nullptr; }
};

//Funcion que verifica si un punto existe en el octree
bool Octree::exist(const Point& p) {
    if (esHoja()) {
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

//Funcion que inserta un punto en el octree
void Octree::insert(const Point& p) {
    if (exist(p)) return;

    if (esHoja()) {
        if (puntos.size() < nPoints) {
            puntos.push_back(p);
            return;
        } else {
            subdividir(); 
        }
    }

    int octante = determinar_octante(bottomLeft, h, p);
    if (children[octante] == nullptr) {
        double h1 = h / 2;
        int x_offset = (octante & 4) ? h1 : 0;
        int y_offset = (octante & 2) ? h1 : 0;
        int z_offset = (octante & 1) ? h1 : 0;
        Point newBottomLeft(bottomLeft.x + x_offset, bottomLeft.y + y_offset, bottomLeft.z + z_offset);
        children[octante] = new Octree(newBottomLeft, h1, nPoints);
    }
    children[octante]->insert(p);
}


//Funcion que busca el punto mas cercano a un punto dado
Point Octree::find_closest(const Point& target, int radius) {
    Point closest;
    double min_dist = numeric_limits<double>::max();
    double radius_squared = radius * radius;

    function<void(Octree*)> search = [&](Octree* node) {
        if (!node) return;

        // Revisar los puntos en este nodo
        for (const auto& p : node->puntos) {
            double dist_squared = pow(p.x - target.x, 2) + pow(p.y - target.y, 2) + pow(p.z - target.z, 2);
            if (dist_squared < min_dist && dist_squared <= radius_squared) {
                min_dist = dist_squared;
                closest = p;
            }
        }

        // Buscar en los nodos hijos
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                search(node->children[i]);
            }
        }
        };

    search(this);

    // Si no se encontró un punto dentro del radio, devolver un punto inválido
    if (min_dist == numeric_limits<double>::max()) {
        cout << "No se encontró un punto dentro del radio especificado.\n";
        return Point(-1, -1, -1);  // Valor indicativo de que no se encontró
    }

    return closest;
}


//Funciones creadas
//Funcion que determina si un nodo es hoja o no
bool Octree::esHoja() {
    for (int i = 0; i < 8; i++) {
        if (children[i] != nullptr) {
            return false;
        }
    }
    return true;
}

//Funcion que subdivide el octree, cuando se llena el nodo
void Octree::subdividir() {
    double h1 = h / 2;
    cout << "Subdividiendo nodo en (" << bottomLeft.x << ", " << bottomLeft.y << ", " << bottomLeft.z << ") con h = " << h << "\n";

    for (int i = 0; i < 8; i++) {
        int x_offset = (i & 4) ? h1 : 0;
        int y_offset = (i & 2) ? h1 : 0;
        int z_offset = (i & 1) ? h1 : 0;
        Point newBottomLeft(bottomLeft.x + x_offset, bottomLeft.y + y_offset, bottomLeft.z + z_offset);
        children[i] = new Octree(newBottomLeft, h1, nPoints);
        cout << "  Nodo hijo " << i << " creado en (" << newBottomLeft.x << ", " << newBottomLeft.y << ", " << newBottomLeft.z << ")\n";
    }

    vector<Point> puntos_a_redistribuir = puntos;
    puntos.clear();

    for (const auto& point : puntos_a_redistribuir) {
        int octante = determinar_octante(bottomLeft, h, point);
        cout << "  Redistribuyendo punto (" << point.x << ", " << point.y << ", " << point.z << ") al octante " << octante << "\n";
        if (children[octante] != nullptr) {
            children[octante]->puntos.push_back(point); // ¡más directo y eficiente!
        } else {
            cout << "Error: Nodo hijo " << octante << " no inicializado.\n";
        }
    }
    
}

//Funcion que cambia la cantidad de puntos para un nodo
void Octree::nueva_cant_nPoint(int newnPoint) {
    nPoints = newnPoint;
    if (!esHoja()) {
        for (int i = 0; i < 8; i++) {
            if (children[i] != nullptr) {
                children[i]->nueva_cant_nPoint(newnPoint);
            }
        }
    }
}

//Funcion que lee el .csv
static vector<Point> leer_csv(const string& filename) {
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

//Funcion para imprimir el Octree
void Octree::imprimir(int nivel) {
    string indent(nivel * 2, ' ');
    cout << indent << " Nodo (BottomLeft: " << bottomLeft.x << ", " << bottomLeft.y << ", " << bottomLeft.z << ") - h: " << h << "\n";

    if (!puntos.empty()) {
        cout << indent << " Puntos en este nodo: \n";
        for (const auto& p : puntos) {
            cout << indent << "    - (" << p.x << ", " << p.y << ", " << p.z << ")\n";
        }
    }

    for (int i = 0; i < 8; i++) {
        if (children[i] != nullptr) {
            cout << indent << " Octante " << i << "\n";
            children[i]->imprimir(nivel + 1);
        }
    }
}

int main() {
    vector<Point> puntos = leer_csv("points2.csv");
    if (puntos.empty()) {
        cout << "No se encontraron puntos en el archivo CSV." << endl;
        return 1;
    }

    int x_min = numeric_limits<int>::max(), x_max = numeric_limits<int>::min();
    int y_min = numeric_limits<int>::max(), y_max = numeric_limits<int>::min();
    int z_min = numeric_limits<int>::max(), z_max = numeric_limits<int>::min();

    for (const auto& p : puntos) {
        x_min = min(x_min, p.x);
        x_max = max(x_max, p.x);
        y_min = min(y_min, p.y);
        y_max = max(y_max, p.y);
        z_min = min(z_min, p.z);
        z_max = max(z_max, p.z);
    }

    Point bottomLeft(x_min, y_min, z_min);
    double h = max({ x_max - x_min, y_max - y_min, z_max - z_min });
    Octree miOctree(bottomLeft, h, 20);

    for (const auto& p : puntos) {
        miOctree.insert(p);
    }

	//miOctree.nueva_cant_nPoint(8);  //Cambia la cantidad de puntos para un nodo
    miOctree.imprimir(0);  //Funcion imprimir Octree

    cout << "BottomLeft: (" << miOctree.getbottomleft().x << ", " << miOctree.getbottomleft().y << ", " << miOctree.getbottomleft().z << ")\n";
    cout << "Altura " << miOctree.getH() << "\n";

    //Funcion para encontrar el punto mas cercano
    
	Point target(18, 69, -179);
	int radio = 10;
    Point closest = miOctree.find_closest(target, radio);

    if (closest.x != -1) {
        cout << "El punto mas cercano a (" << target.x << ", " << target.y << ", " << target.z << ") dentro del radio "
            << radio << " es (" << closest.x << ", " << closest.y << ", " << closest.z << ")\n";
    }
    
	return 0;
}