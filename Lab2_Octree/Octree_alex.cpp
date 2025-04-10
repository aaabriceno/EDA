#include "Octree_alex.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

bool Point::inside(const Point &p, int h) const{
    return p.x>=x && p.x<x+h &&
            p.y>=y && p.y<y+h &&
            p.z>=z && p.z<z+h;
}

double  Point::distance(const Point &p) const{
    return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2) + pow(z - p.z, 2));
}

Octree::Octree(const Point &p, double height, int capacity){
    bottomLeft = p;
    h = height;
    nPoints = capacity;

    for (int i = 0; i < 8; i++)
        children[i] = nullptr;
}

bool Octree::exist(const Point& p) {
    for (const auto& point : points) { 
        if (point == p) 
            return true;
    }

    for (int i = 0; i < 8; i++) {
        if (children[i] != nullptr && children[i]->exist(p))
            return true;
    }

    return false;
}


void Octree::insert(const Point &new_point){
    if(!bottomLeft.inside(new_point, h) || exist(new_point)){
        return;
    }

    if(points.size()<nPoints){
        points.push_back(new_point);
    }
    else{
        if(!children[0]){
            double mid_h = h/2;
            int z = bottomLeft.z, index=0;
            for(int i=0; i<2; i++){
                int y = bottomLeft.y;
                for(int j=0; j<2; j++){
                    int x = bottomLeft.x;
                    for(int k=0; k<2; k++){
                        children[index++] = new Octree(Point(x,y,z), mid_h, nPoints);
                        x+=mid_h;
                    }
                    y+=mid_h;
                }
                z+=mid_h;
            }
        }

        for (int i = 0; i < 8; i++) {
            if (children[i]->bottomLeft.inside(new_point, children[i]->h)) {
                children[i]->insert(new_point);
                return;
            }
        }

    }
}

void Octree::printTree(std::string line) {
    if (points.size()) {
      std::cout << line;
      for(int i=0; i<points.size(); i++){
        std::cout<<points[i]<<" bottom: "<<bottomLeft<<" ";
      }
      std::cout<<std::endl;
    }
    for (int i = 0; i < 8; i++) {
        if (children[i]) {
            children[i]->printTree(line + "-");
        }
    }
}

void Octree::find_closest(const Point &target, int radius, Point &closestPoint, double &minDist) {
    for (const auto &p : points) {
        double dist = p.distance(target);
        if (dist < minDist && dist <= radius) {
            minDist = dist;
            closestPoint = p;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (children[i] != nullptr) {
            children[i]->find_closest(target, radius, closestPoint, minDist);
        }
    }
}

Point Octree::find_closest(const Point &target, int radius) {
    Point closestPoint;
    double minDist = std::numeric_limits<double>::max();

    find_closest(target, radius, closestPoint, minDist);

    if (!closestPoint.init) {
        cout << "No se encontró un punto cercano." << endl;
    } else {
        cout << "Punto más cercano: " << closestPoint <<" with distance: "<<closestPoint.distance(target)<< endl;
    }
    return closestPoint;
}

void Octree::get_h_bottom(const Point &p){
    for(int i=0; i<points.size(); i++){
        if(p==points[i]){
            cout<<"bottomLeft: "<<bottomLeft<<" h: "<<h<<endl;
            return;
        }
    }

    for(int i=0; i<8; i++){
        if(children[i]!=nullptr)
            children[i]->get_h_bottom(p);
    }
}


void leerCSV(const std::string &archivo, std::vector<int> &columna1,
    std::vector<int> &columna2, std::vector<int> &columna3) {

std::ifstream file(archivo);

if (!file.is_open()) {
std::cerr << "Error to open file." << std::endl;
return;
}

std::string linea;
while (std::getline(file, linea)) {
std::istringstream ss(linea);
std::string valor;
std::getline(ss, valor, ',');
int valor1 = std::stoi(valor);
columna1.push_back(valor1);

std::getline(ss, valor, ',');
int valor2 = std::stoi(valor);
columna2.push_back(valor2);

std::getline(ss, valor, ',');
int valor3 = std::stoi(valor);
columna3.push_back(valor3);
}

file.close();
}

int max_num(int a,int b,int c){
    return a>b ? (a>c? a:c) : (b>c? b:c);
  }
  
  int get_h(vector<int> X, vector<int> Y, vector<int> Z, Point &p_aux) {
    p_aux = Point(
      *std::min_element(X.begin(),X.end()),
      *std::min_element(Y.begin(),Y.end()),
      *std::min_element(Z.begin(),Z.end())
    );
  
    int max_x = *std::max_element(X.begin(),X.end()), 
    max_y = *std::max_element(Y.begin(),Y.end()), 
    max_z = *std::max_element(Z.begin(),Z.end());
  
  //   cout<<max_x<<" "<<p_aux->x<<endl;
  //   cout<<max_y<<" "<<p_aux->y<<endl;
  //   cout<<max_z<<" "<<p_aux->z<<endl;
    
    return max_num(
      max_x >= 0 ? max_x - p_aux.x : max_x - p_aux.x, 
      max_y >= 0 ? max_y - p_aux.y : max_x - p_aux.y, 
      max_z >= 0 ? max_z - p_aux.z : max_x - p_aux.z
    );
  }
  
  
  int main() {
    // vector<int> X({9,12,3,-10,5,11,13,14,-14,-13,-11}),
    // Y({3,6,8,7,4,9,12,3,-9,11,6}), Z({10,7,5,1,9,3,9,15,-12,2,8});
    std::vector<int> X;
    std::vector<int> Y;
    std::vector<int> Z;
  
    leerCSV("points2.csv", X, Y, Z);
    // leerCSV("points2.csv", X, Y, Z);
    Point p_aux;
    int h = get_h(X, Y, Z, p_aux), N;
    cout<<"BottomLeft: "<<p_aux<<" h: "<<h<<endl;
    cout<<"Enter N points in the node: ";cin>>N;
  
    Octree octree(p_aux, h, N);
  
    for (int i = 0; i < X.size(); i++) {
      octree.insert(Point(X[i], Y[i], Z[i]));
    }
  
    std::cout << "Estructura del Octree:" << std::endl;
    octree.printTree();
  
    Point p_2find=Point(0,0,0);
    int radius=500;
    cout<<"Point exist in the tree? ";
    cout<<octree.exist(p_2find)<<endl;
  
    std::cout << "\nFind closest del Octree:" << std::endl;
    Point found = octree.find_closest(p_2find, radius);
  
    octree.get_h_bottom(found);
  
    while (1){
      int a,b,c, radius;
      cin>>a;
      cin>>b;
      cin>>c;
      Point p_find=Point(a,b,c);
      cin>>radius;
      cout<<"Point exist in the tree? ";
      cout<<octree.exist(p_find)<<endl;
  
      std::cout << "\nFind closest del Octree:" << std::endl;
      Point found2 = octree.find_closest(p_find, radius);
      octree.get_h_bottom(found2);
    }
    return 0;
  };