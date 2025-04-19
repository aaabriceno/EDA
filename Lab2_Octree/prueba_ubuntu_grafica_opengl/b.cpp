#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

// Estructura Point (con algunas mejoras)
struct Point {
    double x, y, z;
    bool init;  // Indicador de inicialización (opcional)
    
    Point(double a = 0, double b = 0, double c = 0) 
        : x(a), y(b), z(c), init(true) {}

    // Sobrecarga del operador "==" para facilitar la comparación
    bool operator==(const Point &other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    // Método que verifica si el punto p está dentro del cubo definido por 
    // este punto (como esquina inferior izquierda) y la longitud de arista h.
    bool inside(const Point &p, double h) const {
        return (p.x >= x && p.x < x + h &&
                p.y >= y && p.y < y + h &&
                p.z >= z && p.z < z + h);
    }

    // Cálculo de la distancia euclidiana
    double distance(const Point &p) const {
        return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2) + pow(z - p.z, 2));
    }
};

// Sobrecarga del operador << para imprimir puntos
ostream& operator<<(ostream& os, const Point& p) {
    os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
    return os;
}

class Octree {
private:
    Point bottomLeft;   // Esquina inferior izquierda del cubo
    double h;           // Longitud de la arista del cubo
    int nPoints;        // Capacidad máxima de puntos por nodo
    vector<Point> points;
    Octree* children[8];  // Nodos hijos

public:
    // Constructor
    Octree(const Point &p, double height, int capacity)
        : bottomLeft(p), h(height), nPoints(capacity) {
        for (int i = 0; i < 8; i++) 
            children[i] = nullptr;
    }

    // Verifica si un punto existe en el octree
    bool exist(const Point &p) {
        // Verificar en el nodo actual
        for (const auto &pt : points)
            if (pt == p)
                return true;
        // Verificar recursivamente en los hijos
        for (int i = 0; i < 8; i++) {
            if (children[i] != nullptr && children[i]->exist(p))
                return true;
        }
        return false;
    }

    // Inserta un punto en el octree
    void insert(const Point &new_point) {
        // Verificar que el punto esté dentro del espacio de este nodo
        // (aquí se reemplaza el uso de la función determinar_octante)
        if (!bottomLeft.inside(new_point, h) || exist(new_point))
            return;

        // Si el nodo tiene capacidad, se inserta directamente
        if (points.size() < nPoints) {
            points.push_back(new_point);
        } 
        else {
            // Si aún no se han creado los hijos, subdividir el nodo
            if (!children[0]) {
                subdividir();
            }
            // Insertar el punto en el hijo correspondiente
            for (int i = 0; i < 8; i++) {
                if (children[i]->bottomLeft.inside(new_point, children[i]->h)) {
                    children[i]->insert(new_point);
                    return;
                }
            }
        }
    }

    // Subdivide el nodo actual en 8 hijos
    void subdividir() {
        double mid_h = h / 2;
        int index = 0;
        // Se crean los 8 hijos usando bucles anidados (para z, y y x)
        for (int dz = 0; dz < 2; dz++) {
            for (int dy = 0; dy < 2; dy++) {
                for (int dx = 0; dx < 2; dx++) {
                    Point newBL(
                        bottomLeft.x + dx * mid_h,
                        bottomLeft.y + dy * mid_h,
                        bottomLeft.z + dz * mid_h
                    );
                    children[index++] = new Octree(newBL, mid_h, nPoints);
                }
            }
        }
        // Reinsertar los puntos del nodo actual en los hijos correspondientes
        vector<Point> old_points = points;
        points.clear();
        for (auto &pt : old_points) {
            for (int i = 0; i < 8; i++) {
                if (children[i]->bottomLeft.inside(pt, children[i]->h)) {
                    children[i]->insert(pt);
                    break;
                }
            }
        }
    }

    // Imprime la estructura del Octree
    void printTree(string indent = "") {
        if (!points.empty()) {
            cout << indent << "Nodo (BottomLeft: " << bottomLeft << ", h: " << h << ") contiene: ";
            for (auto &pt : points) 
                cout << pt << " ";
            cout << endl;
        }
        for (int i = 0; i < 8; i++) {
            if (children[i])
                children[i]->printTree(indent + "  ");
        }
    }

    // Función auxiliar para encontrar el punto más cercano (similar al enfoque de tu amigo)
    void find_closest(const Point &target, int radius, Point &closestPoint, double &minDist) {
        // Revisar puntos en el nodo actual
        for (const auto &pt : points) {
            double dist = pt.distance(target);
            if (dist < minDist && dist <= radius) {
                minDist = dist;
                closestPoint = pt;
            }
        }
        // Revisar recursivamente en los nodos hijos
        for (int i = 0; i < 8; i++) {
            if (children[i] != nullptr) {
                children[i]->find_closest(target, radius, closestPoint, minDist);
            }
        }
    }

    // Versión pública de find_closest, que muestra el resultado
    Point find_closest(const Point &target, int radius) {
        Point closestPoint(0,0,0);
        double minDist = numeric_limits<double>::max();
        find_closest(target, radius, closestPoint, minDist);
        if (minDist == numeric_limits<double>::max()) {
            cout << "No se encontró un punto dentro del radio especificado." << endl;
        } else {
            cout << "Punto más cercano a " << target 
                 << " es " << closestPoint 
                 << " con distancia " << minDist << endl;
        }
        return closestPoint;
    }

    // Métodos getters para utilizar en la impresión o validaciones externas
    Point getBottomLeft() const { return bottomLeft; }
    double getH() const { return h; }


    void obtenerHojas(vector<Octree*>& hojas) {
        bool esHoja = true;
        for (int i = 0; i < 8; i++) {
            if (children[i] != nullptr) {
                esHoja = false;
                children[i]->obtenerHojas(hojas);
            }
        }
        if (esHoja) {
            hojas.push_back(this);
        }
    }
};

// Función para leer puntos desde un CSV
vector<Point> leer_csv(const string &filename) {
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

/*
int main() {
    vector<Point> puntos = leer_csv("points2.csv");
    if (puntos.empty()){
        cout << "No se encontraron puntos en el archivo CSV." << endl;
        return 1;
    }

    // Determinar el mínimo y máximo de las coordenadas para obtener la región (bottomLeft y h)
    double x_min = numeric_limits<double>::max(), x_max = numeric_limits<double>::min();
    double y_min = numeric_limits<double>::max(), y_max = numeric_limits<double>::min();
    double z_min = numeric_limits<double>::max(), z_max = numeric_limits<double>::min();

    for (auto &p : puntos) {
        x_min = min(x_min, p.x);
        x_max = max(x_max, p.x);
        y_min = min(y_min, p.y);
        y_max = max(y_max, p.y);
        z_min = min(z_min, p.z);
        z_max = max(z_max, p.z);
    }
    Point bottomLeft(x_min, y_min, z_min);
    double h = max({x_max - x_min, y_max - y_min, z_max - z_min});
    
    // Lectura de la capacidad mínima (número de puntos por nodo)
    int capacity;
    cout << "Ingrese la cantidad de puntos por nodo: ";
    cin >> capacity;

    Octree miOctree(bottomLeft, h, capacity);
    for (auto &p : puntos)
        miOctree.insert(p);

    miOctree.printTree();

    cout << "BottomLeft del octree: " << miOctree.getBottomLeft() << endl;
    cout << "Altura (h): " << miOctree.getH() << endl;

    // Prueba de búsqueda del punto más cercano
    Point target(18, 69, -179);
    int radio = 300;
    miOctree.find_closest(target, radio);

    return 0;
}
*/