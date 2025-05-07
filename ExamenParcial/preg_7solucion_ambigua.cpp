#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>

using namespace std;

const int MAX_ENTRIES = 4;

struct Rect {
    double minX, minY, maxX, maxY;
    Rect(double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0)
        : minX(x1), minY(y1), maxX(x2), maxY(y2) {}

    double area() const {
        return (maxX - minX) * (maxY - minY);
    }

    Rect combine(const Rect& other) const {
        return Rect(
            min(minX, other.minX),
            min(minY, other.minY),
            max(maxX, other.maxX),
            max(maxY, other.maxY)
        );
    }

    void print() const {
        cout << "[" << minX << "," << minY << " - " << maxX << "," << maxY << "] ";
    }
};

struct Entry {
    Rect rect;
    class Node* child = nullptr;
    Entry(Rect r, class Node* c = nullptr) : rect(r), child(c) {}
};

struct Node {
    vector<Entry> entries;
    bool isLeaf;
    Node(bool leaf = true) : isLeaf(leaf) {}
};

enum SplitType { SIMPLE, LINEAR, QUADRATIC };

class RTree {
public:
    Node* root;
    SplitType splitType = SIMPLE;

    RTree() {
        root = new Node(true);
    }

    void insert(Rect r) {
        Node* node = chooseLeaf(root, r);
        node->entries.push_back(Entry(r));
        if (node->entries.size() > MAX_ENTRIES) {
            Node* newNode = splitNode(node);
            adjustTree(node, newNode);
        }
    }

    void imprimir(Node* node = nullptr, int depth = 0) {
        if (!node) node = root;
        for (int i = 0; i < depth; ++i) cout << "  ";
        cout << (node->isLeaf ? "Leaf: " : "Node: ");
        for (auto& e : node->entries) e.rect.print();
        cout << endl;
        if (!node->isLeaf) {
            for (auto& e : node->entries) imprimir(e.child, depth + 1);
        }
    }

private:
    Node* chooseLeaf(Node* node, Rect r) {
        if (node->isLeaf) return node;
        double minInc = 1e9;
        Entry* best = nullptr;
        for (auto& e : node->entries) {
            double oldArea = e.rect.area();
            double newArea = e.rect.combine(r).area();
            double inc = newArea - oldArea;
            if (inc < minInc) {
                minInc = inc;
                best = &e;
            }
        }
        return chooseLeaf(best->child, r);
    }

    void adjustTree(Node* n, Node* nn) {
        if (n == root) {
            Node* newRoot = new Node(false);
            newRoot->entries.push_back(Entry(boundingRect(n->entries), n));
            newRoot->entries.push_back(Entry(boundingRect(nn->entries), nn));
            root = newRoot;
            return;
        }
        // Simplified adjustTree, assumes 2-level tree for demonstration
    }

    Rect boundingRect(const vector<Entry>& es) {
        Rect r = es[0].rect;
        for (int i = 1; i < es.size(); ++i)
            r = r.combine(es[i].rect);
        return r;
    }

    Node* splitNode(Node* node) {
        if (splitType == LINEAR) return linearSplitNode(node);
        if (splitType == QUADRATIC) return quadraticSplitNode(node);
        return simpleSplitNode(node);
    }

    Node* simpleSplitNode(Node* node) {
        vector<Entry>& es = node->entries;
        sort(es.begin(), es.end(), [](const Entry& a, const Entry& b) {
            return a.rect.minX < b.rect.minX;
        });
        Node* sibling = new Node(node->isLeaf);
        int split = es.size() / 2;
        sibling->entries.assign(es.begin() + split, es.end());
        es.erase(es.begin() + split, es.end());
        return sibling;
    }

    Node* linearSplitNode(Node* node) {
        vector<Entry> es = node->entries;
        Node* n1 = new Node(node->isLeaf);
        Node* n2 = new Node(node->isLeaf);
        int seed1 = -1, seed2 = -1;
        double maxDist = -1;
        for (int i = 0; i < es.size(); ++i) {
            for (int j = i + 1; j < es.size(); ++j) {
                Rect r = es[i].rect.combine(es[j].rect);
                double d = r.area() - es[i].rect.area() - es[j].rect.area();
                if (d > maxDist) {
                    maxDist = d;
                    seed1 = i;
                    seed2 = j;
                }
            }
        }
        n1->entries.push_back(es[seed1]);
        n2->entries.push_back(es[seed2]);
        vector<bool> assigned(es.size(), false);
        assigned[seed1] = assigned[seed2] = true;
        while (n1->entries.size() + n2->entries.size() < es.size()) {
            int next = -1;
            double maxDiff = -1;
            bool toGroup1 = true;
            for (int i = 0; i < es.size(); ++i) {
                if (assigned[i]) continue;
                Rect r = es[i].rect;
                Rect r1 = boundingRect(n1->entries).combine(r);
                Rect r2 = boundingRect(n2->entries).combine(r);
                double diff = abs(r1.area() - boundingRect(n1->entries).area()) -
                              abs(r2.area() - boundingRect(n2->entries).area());
                if (abs(diff) > maxDiff) {
                    maxDiff = abs(diff);
                    next = i;
                    toGroup1 = diff < 0;
                }
            }
            if (toGroup1)
                n1->entries.push_back(es[next]);
            else
                n2->entries.push_back(es[next]);
            assigned[next] = true;
        }
        node->entries = n1->entries;
        return n2;
    }

    Node* quadraticSplitNode(Node* node) {
        vector<Entry> es = node->entries;
        Node* n1 = new Node(node->isLeaf);
        Node* n2 = new Node(node->isLeaf);
        int seed1 = 0, seed2 = 0;
        double worstWaste = -1;
        for (int i = 0; i < es.size(); ++i) {
            for (int j = i + 1; j < es.size(); ++j) {
                Rect r = es[i].rect.combine(es[j].rect);
                double waste = r.area() - es[i].rect.area() - es[j].rect.area();
                if (waste > worstWaste) {
                    worstWaste = waste;
                    seed1 = i;
                    seed2 = j;
                }
            }
        }
        n1->entries.push_back(es[seed1]);
        n2->entries.push_back(es[seed2]);
        vector<bool> assigned(es.size(), false);
        assigned[seed1] = assigned[seed2] = true;
        while (n1->entries.size() + n2->entries.size() < es.size()) {
            int next = -1;
            double maxDiff = -1;
            bool toGroup1 = true;
            for (int i = 0; i < es.size(); ++i) {
                if (assigned[i]) continue;
                Rect r = es[i].rect;
                Rect r1 = boundingRect(n1->entries).combine(r);
                Rect r2 = boundingRect(n2->entries).combine(r);
                double d1 = r1.area() - boundingRect(n1->entries).area();
                double d2 = r2.area() - boundingRect(n2->entries).area();
                double diff = abs(d1 - d2);
                if (diff > maxDiff) {
                    maxDiff = diff;
                    next = i;
                    toGroup1 = d1 < d2;
                }
            }
            if (toGroup1)
                n1->entries.push_back(es[next]);
            else
                n2->entries.push_back(es[next]);
            assigned[next] = true;
        }
        node->entries = n1->entries;
        return n2;
    }
};

int main() {
    srand(time(0));
    vector<Rect> rects;
    for (int i = 0; i < 25; ++i) {
        double x1 = rand() % 95;
        double y1 = rand() % 95;
        double length = 1 + (rand() % 5);
        double x2 = min(100.0, x1 + length);
        double y2 = min(100.0, y1 + length);
        rects.push_back(Rect(x1, y1, x2, y2));
    }

    for (SplitType type : {SIMPLE, LINEAR, QUADRATIC}) {
        cout << "\n\n=========== " << (type == SIMPLE ? "SIMPLE" : type == LINEAR ? "LINEAR" : "QUADRATIC") << " SPLIT ===========\n";
        RTree tree;
        tree.splitType = type;
        for (auto& r : rects) {
            tree.insert(r);
        }
        tree.imprimir();
    }

    return 0;
}