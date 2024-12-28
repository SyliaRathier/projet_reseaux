#include "hexagon.h"
#include <QPainter>
#include <cmath>

Hexagon::Hexagon(float x, float y, float radius) : x(x), y(y), radius(radius) {}

void Hexagon::draw(QPainter &painter) const {
    // Set the painter's composition mode to source over, so it doesn't affect the background
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // Set opacity to 20% (can be between 0 and 1)
    painter.setOpacity(1);

    // Set the brush color to white
    painter.setBrush(Qt::lightGray);

    // Set the pen color to black for the hexagon outline
    painter.setPen(Qt::white);

    // Calculate the points of the hexagon
    QPointF points[6];
    for (int i = 0; i < 6; ++i) {
        double angle = M_PI / 3 * i; // 60 degrees in radians
        points[i] = QPointF(x + radius * cos(angle), y + radius * sin(angle)); // Adjust the size
    }

    // Draw the hexagon
    painter.drawPolygon(points, 6);
}
