#include "box.hpp"

namespace gcn {

Box::~Box()
{
}

void Box::setSpacing(int spacing)
{
    mSpacing = spacing;
}

int Box::getSpacing() const
{
    return mSpacing;
}

Box::Box(int spacing) : mSpacing(spacing)
{
    setOpaque(false);
}

VBox::VBox(int spacing): Box(spacing)
{
}

VBox::~VBox()
{
}

void VBox::pack(Widget* child, int padding)
{
    int x = 0, y = 0;
    WidgetPadding::const_iterator I = mWidgetPadding.begin();
    WidgetPadding::const_iterator Iend = mWidgetPadding.end();
    WidgetList::const_iterator J = mWidgets.begin();
    WidgetList::const_iterator Jend = mWidgets.end();
    for (; I != Iend && J != Jend; ++I, ++J) {
        x = std::max(x, (*J)->getWidth());
        y += *I * 2;
        y += (*J)->getHeight();
        y += mSpacing;
    }
    y += padding;

    add(child, 0, y);
    mWidgetPadding.push_back(padding);

    x = std::max(x, child->getWidth());
    y += child->getHeight();
    y += padding;

    setSize(x, y);
}

HBox::HBox(int spacing): Box(spacing)
{
}

HBox::~HBox()
{
}

void HBox::pack(Widget* child, int padding)
{
    // FIXME This is so similar to VBox::pack we should move pack into Box,
    // and virtualise the coord handling bit.
    int x = 0, y = 0;
    WidgetPadding::const_iterator I = mWidgetPadding.begin();
    WidgetPadding::const_iterator Iend = mWidgetPadding.end();
    WidgetList::const_iterator J = mWidgets.begin();
    WidgetList::const_iterator Jend = mWidgets.end();
    for (; I != Iend && J != Jend; ++I, ++J) {
        x += *I * 2;
        x += (*J)->getWidth();
        x += mSpacing;
        y = std::max(y, (*J)->getHeight());
    }
    x += padding;

    add(child, x, 0);
    mWidgetPadding.push_back(padding);

    x += child->getWidth();
    x += padding;
    y = std::max(y, child->getHeight());

    setSize(x, y);
}

} // end gcn
