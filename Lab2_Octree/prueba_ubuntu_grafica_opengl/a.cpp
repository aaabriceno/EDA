#include <GL/glut.h>
#include <vector>
#include "b.cpp"  // O usa encabezados separados: Point.hpp, Octree.hpp, etc.

float cameraPosX = 100.0f, cameraPosY = 100.0f, cameraPosZ = 300.0f;  // Posición inicial de la cámara
float cameraSpeed = 5.0f; 

Octree* octree;
vector<Octree*> hojas;

void dibujarCubo(const Point& bottomLeft, double h) {
    double x = bottomLeft.x;
    double y = bottomLeft.y;
    double z = bottomLeft.z;

    glBegin(GL_LINES);
    // Aristas inferiores
    glVertex3f(x, y, z); glVertex3f(x + h, y, z);
    glVertex3f(x + h, y, z); glVertex3f(x + h, y + h, z);
    glVertex3f(x + h, y + h, z); glVertex3f(x, y + h, z);
    glVertex3f(x, y + h, z); glVertex3f(x, y, z);

    // Aristas superiores
    glVertex3f(x, y, z + h); glVertex3f(x + h, y, z + h);
    glVertex3f(x + h, y, z + h); glVertex3f(x + h, y + h, z + h);
    glVertex3f(x + h, y + h, z + h); glVertex3f(x, y + h, z + h);
    glVertex3f(x, y + h, z + h); glVertex3f(x, y, z + h);

    // Aristas verticales
    glVertex3f(x, y, z); glVertex3f(x, y, z + h);
    glVertex3f(x + h, y, z); glVertex3f(x + h, y, z + h);
    glVertex3f(x + h, y + h, z); glVertex3f(x + h, y + h, z + h);
    glVertex3f(x, y + h, z); glVertex3f(x, y + h, z + h);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Cambiar la vista de la cámara
    gluLookAt(cameraPosX, cameraPosY, cameraPosZ, 0, 0, 0, 0, 1, 0);

    glColor3f(1.0, 1.0, 1.0);
    for (auto hoja : hojas) {
        dibujarCubo(hoja->getBottomLeft(), hoja->getH());
    }

    glutSwapBuffers();
}

bool keys[256];  // Para saber qué teclas están presionadas

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;
    if (key == 27) exit(0);  // Salir con Esc
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;  // Cuando se suelta la tecla
}

void idle() {
    if (keys['w']) cameraPosZ -= cameraSpeed;
    if (keys['s']) cameraPosZ += cameraSpeed;
    if (keys['a']) cameraPosX -= cameraSpeed;
    if (keys['d']) cameraPosX += cameraSpeed;
    if (keys['q']) cameraPosY += cameraSpeed;
    if (keys['e']) cameraPosY -= cameraSpeed;
    
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    vector<Point> puntos = leer_csv("points2.csv");
    if (puntos.empty()) {
        cout << "No se encontraron puntos en el archivo CSV." << endl;
        return 1;
    }

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
    int capacity = 1; // Puedes cambiar esto

    octree = new Octree(bottomLeft, h, capacity);
    for (auto &p : puntos)
        octree->insert(p);

    // Obtener hojas
    octree->obtenerHojas(hojas);

    // Inicializar GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Visualización del Octree");

    glEnable(GL_DEPTH_TEST);

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutIdleFunc(idle);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);  // Registrar la función de teclado

    glutMainLoop();

    return 0;
}
