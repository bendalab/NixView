#include "colormap.hpp"

ColorMap::ColorMap()
{
    current = 0;
    cmap = {QColor(0, 0, 102), QColor(0, 102, 51), QColor(204, 0, 0), QColor(0, 204, 204),
            QColor(102, 0, 102), QColor(204, 102, 0), QColor(0, 0, 0), QColor(51, 0, 102),
            QColor(51, 255, 51), QColor(255, 0, 255)};
}


QColor ColorMap::next() {
    QColor c = cmap[current];
    current++;
    if (current >= cmap.size())
        current = 0;

    return c;

}
