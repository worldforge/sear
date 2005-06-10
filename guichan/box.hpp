#ifndef GCN_BOX_HPP
#define GCN_BOX_HPP

#include <guichan/widgets/container.hpp>

namespace gcn
{
    /**
     * This is the box container base class. It is a widget that holds other
     * widgets in an ordered row or column. It is abstract and should not be
     * instantiated. The pattern is from gtk+, including some of the comments.
     */
    class GCN_CORE_DECLSPEC Box: public Container
    {
    public:

        /**
         * Destructor
         */
        virtual ~Box();

        /**
         * Set the spacing between widgets. Default set in the constructor
         * of classes which inherit from Box, and is typically zero.
         */
        void setSpacing(int spacing);

        /**
         * Return the value set by setSpacing.
         */
        int getSpacing() const;

        /**
         * Insert a widget onto the end of the box.
         * @param child A Widget to be added to box.
         * @padding Padding that is added on either side of the widget. This is different to spacing set when the box is created - spacing is added between objects, and padding is added on either side of an object. In order to work correctly widgets passed to this function should already have the size they will have when they are to be rendered. The current implementation does not have the capability to redo the layout when the size of child widgets changes, or the spacing or padding are changed. The effect on spacing if setSpacing() is called after widgets have been added is undefined.
         */
        virtual void pack(Widget* child, int padding = 0) = 0;

    protected:

        /**
         * Constructor. A Box container is transparent as default. This constructor can only be called by classes which inherit from this one.
         *
         * @see setOpaque, isOpaque
         */
        explicit Box(int spacing = 0);

        typedef std::list<int> WidgetPadding;
        int mSpacing;
        WidgetPadding mWidgetPadding;
    }; // end Box

    class GCN_CORE_DECLSPEC VBox: public Box
    {
    public:
        explicit VBox(int spacing = 0);

        virtual ~VBox();

        virtual void pack(Widget* child, int padding = 0);
    };

    class GCN_CORE_DECLSPEC HBox: public Box
    {
    public:
        explicit HBox(int spacing = 0);

        virtual ~HBox();

        virtual void pack(Widget* child, int padding = 0);
    }; // end VBox
} // end gcn

#endif // end GCN_BOX_HPP
