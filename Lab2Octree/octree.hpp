#ifndef OCTREE_H
#define OCTREE_H
#include <vector>
using namespace std;

struct Point{
    int x;
    int y;
    int z;

   Point() : x(0), y(0), z(0) {}
   Point(int a, int b, int c) : x(a), y(b), z(c) {}
};


class Octree {
private:
   Octree *children[8];
   vector<Point> puntos;  //Point *points; Vector de nombre "puntos"
   
   Point bottomLeft;  // bottomLeft y h definen el espacio(cubo más grande)
   double h;
   int nPoints; // puntos ingresados.

public:
   /*Funciones dadas*/
   Octree(const Point &p, double height, int capacity);
   bool exist( const Point & );
   void insert( const Point & );
   Point find_closest( const Point &, int radius);
   
   //Funciones creadas
   bool esHoja();
   void subdividir();
   void nueva_cant_nPoint(int newnPoint);
   void imprimir(int nivel);
   double getH() const {return h;}
   Point getbottomleft() const {return bottomLeft;}
};

#endif
