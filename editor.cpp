
#include <graphics.h>
#include <conio.h>
#include <cmath>
#include <vector>
using namespace std;

const double PI = 3.141592653589793;

// Clipping window boundaries
const int LEFT = 300;
const int TOP = 150;
const int RIGHT = 600;
const int BOTTOM = 400;

struct Point {
    double x, y;
};

// Original polygon
vector<Point> original = {
    {250, 200},
    {350, 160},
    {420, 240},
    {370, 330},
    {260, 310}
};

vector<Point> polygon = original;

// Check whether a point is inside a clipping boundary
bool inside(Point p, int edge) {
    if (edge == 0) return p.x >= LEFT;
    if (edge == 1) return p.x <= RIGHT;
    if (edge == 2) return p.y >= TOP;
    return p.y <= BOTTOM;
}

// Find intersection with a clipping boundary
Point intersection(Point a, Point b, int edge) {
    Point p;
    double t;

    if (edge < 2) {
        double x = (edge == 0) ? LEFT : RIGHT;
        t = (x - a.x) / (b.x - a.x);

        p.x = x;
        p.y = a.y + t * (b.y - a.y);
    } else {
        double y = (edge == 2) ? TOP : BOTTOM;
        t = (y - a.y) / (b.y - a.y);

        p.y = y;
        p.x = a.x + t * (b.x - a.x);
    }

    return p;
}

// Clip polygon against one boundary
vector<Point> clipEdge(vector<Point> input, int edge) {
    vector<Point> output;

    if (input.empty())
        return output;

    Point previous = input.back();

    for (Point current : input) {
        bool previousInside = inside(previous, edge);
        bool currentInside = inside(current, edge);

        if (currentInside) {
            if (!previousInside) {
                output.push_back(
                    intersection(previous, current, edge)
                );
            }

            output.push_back(current);
        } else if (previousInside) {
            output.push_back(
                intersection(previous, current, edge)
            );
        }

        previous = current;
    }

    return output;
}

// Sutherland-Hodgman polygon clipping
vector<Point> clipPolygon(vector<Point> input) {
    for (int edge = 0; edge < 4; edge++) {
        input = clipEdge(input, edge);
    }

    return input;
}

// Draw polygon outline
void drawPolygon(vector<Point> p, int color) {
    if (p.size() < 2)
        return;

    setcolor(color);

    for (int i = 0; i < (int)p.size(); i++) {
        int next = (i + 1) % p.size();

        line(
            (int)p[i].x, (int)p[i].y,
            (int)p[next].x, (int)p[next].y
        );
    }
}

// Translation
void translatePolygon(int dx, int dy) {
    for (Point &p : polygon) {
        p.x += dx;
        p.y += dy;
    }
}

// Rotation around polygon center
void rotatePolygon(double degrees) {
    if (polygon.empty())
        return;

    double cx = 0, cy = 0;

    for (Point p : polygon) {
        cx += p.x;
        cy += p.y;
    }

    cx /= polygon.size();
    cy /= polygon.size();

    double angle = degrees * PI / 180.0;

    for (Point &p : polygon) {
        double x = p.x - cx;
        double y = p.y - cy;

        p.x = cx + x * cos(angle) - y * sin(angle);
        p.y = cy + x * sin(angle) + y * cos(angle);
    }
}

// Scaling around polygon center
void scalePolygon(double factor) {
    if (polygon.empty())
        return;

    double cx = 0, cy = 0;

    for (Point p : polygon) {
        cx += p.x;
        cy += p.y;
    }

    cx /= polygon.size();
    cy /= polygon.size();

    for (Point &p : polygon) {
        p.x = cx + (p.x - cx) * factor;
        p.y = cy + (p.y - cy) * factor;
    }
}

// Draw editor screen
void drawScreen(bool clipped) {
    setbkcolor(WHITE);
    cleardevice();

    setcolor(BLACK);
    outtextxy(210, 20, "INTERACTIVE 2-D GRAPHICS EDITOR");

    outtextxy(30, 65, "Arrow keys: Move");
    outtextxy(30, 90, "A / D: Rotate");
    outtextxy(30, 115, "+ / -: Scale");
    outtextxy(30, 140, "C: Clip polygon");
    outtextxy(30, 165, "R: Reset");
    outtextxy(30, 190, "ESC: Exit");

    // Clipping window
    setcolor(BLUE);
    rectangle(LEFT, TOP, RIGHT, BOTTOM);
    outtextxy(LEFT, BOTTOM + 10, "Clipping Window");

    if (clipped) {
        vector<Point> result = clipPolygon(polygon);
        drawPolygon(result, GREEN);

        setcolor(GREEN);
        outtextxy(520, 440, "Green: Clipped Polygon");
    } else {
        drawPolygon(polygon, RED);

        setcolor(RED);
        outtextxy(520, 440, "Red: Original Polygon");
    }
}

int main() {
    initwindow(900, 550, "Interactive 2-D Graphics Editor");

    bool running = true;
    bool clipped = false;

    while (running) {
        drawScreen(clipped);

        int key = getch();

        // Arrow keys
        if (key == 0 || key == 224) {
            key = getch();
            clipped = false;

            if (key == 75) translatePolygon(-10, 0);
            if (key == 77) translatePolygon(10, 0);
            if (key == 72) translatePolygon(0, -10);
            if (key == 80) translatePolygon(0, 10);
        } else {
            switch (key) {
                case 'a':
                case 'A':
                    rotatePolygon(-10);
                    clipped = false;
                    break;

                case 'd':
                case 'D':
                    rotatePolygon(10);
                    clipped = false;
                    break;

                case '+':
                case '=':
                    scalePolygon(1.1);
                    clipped = false;
                    break;

                case '-':
                case '_':
                    scalePolygon(0.9);
                    clipped = false;
                    break;

                case 'c':
                case 'C':
                    clipped = true;
                    break;

                case 'r':
                case 'R':
                    polygon = original;
                    clipped = false;
                    break;

                case 27:
                    running = false;
                    break;
            }
        }
    }

    closegraph();
    return 0;
}
