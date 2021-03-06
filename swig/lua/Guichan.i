%module Guichan

%{
#include <guichan/color.hpp>
#include <guichan/actionevent.hpp>
#include <guichan/actionlistener.hpp>
#include <guichan/basiccontainer.hpp>
#include <guichan/cliprectangle.hpp>
#include <guichan/deathlistener.hpp>
#include <guichan/event.hpp>
#include <guichan/exception.hpp>
#include <guichan/focushandler.hpp>
#include <guichan/focuslistener.hpp>
#include <guichan/font.hpp>
#include <guichan/genericinput.hpp>
#include <guichan/graphics.hpp>
#include <guichan/gui.hpp>
#include <guichan/image.hpp>
#include <guichan/imagefont.hpp>
#include <guichan/imageloader.hpp>
#include <guichan/input.hpp>
#include <guichan/inputevent.hpp>
#include <guichan/key.hpp>
#include <guichan/keyevent.hpp>
#include <guichan/keyinput.hpp>
#include <guichan/keylistener.hpp>
#include <guichan/listmodel.hpp>
#include <guichan/mouseevent.hpp>
#include <guichan/mouseinput.hpp>
#include <guichan/mouselistener.hpp>
#include <guichan/rectangle.hpp>
#include <guichan/selectionevent.hpp>
#include <guichan/selectionlistener.hpp>
#include <guichan/widget.hpp>
#include <guichan/widgetlistener.hpp>
#include <guichan/widgets/button.hpp>
#include <guichan/widgets/checkbox.hpp>
#include <guichan/widgets/container.hpp>
#include <guichan/widgets/dropdown.hpp>
#include <guichan/widgets/icon.hpp>
#include <guichan/widgets/imagebutton.hpp>
#include <guichan/widgets/label.hpp>
#include <guichan/widgets/listbox.hpp>
#include <guichan/widgets/scrollarea.hpp>
#include <guichan/widgets/slider.hpp>
#include <guichan/widgets/radiobutton.hpp>
#include <guichan/widgets/tab.hpp>
#include <guichan/widgets/tabbedarea.hpp>
#include <guichan/widgets/textbox.hpp>
#include <guichan/widgets/textfield.hpp>
#include <guichan/widgets/window.hpp>
%}


%include <std_string.i>

## Note: These have been re-ordered slightly to avoid warnings
%include <guichan/color.hpp>
%include <guichan/event.hpp>
%include <guichan/rectangle.hpp>
%include <guichan/input.hpp>
%include <guichan/actionevent.hpp>
%include <guichan/actionlistener.hpp>
%include <guichan/widget.hpp>
%include <guichan/deathlistener.hpp>
%include <guichan/basiccontainer.hpp>
%include <guichan/cliprectangle.hpp>
%include <guichan/exception.hpp>
%include <guichan/focushandler.hpp>
%include <guichan/focuslistener.hpp>
%include <guichan/font.hpp>
%include <guichan/genericinput.hpp>
%include <guichan/graphics.hpp>
%include <guichan/gui.hpp>
%include <guichan/image.hpp>
%include <guichan/imagefont.hpp>
%include <guichan/imageloader.hpp>
%include <guichan/inputevent.hpp>
%include <guichan/key.hpp>
%include <guichan/keyevent.hpp>
%include <guichan/keyinput.hpp>
%include <guichan/keylistener.hpp>
%include <guichan/listmodel.hpp>
%include <guichan/mouseevent.hpp>
%include <guichan/mouseinput.hpp>
%include <guichan/mouselistener.hpp>
%include <guichan/selectionevent.hpp>
%include <guichan/selectionlistener.hpp>
%include <guichan/widgetlistener.hpp>
%include <guichan/widgets/button.hpp>
%include <guichan/widgets/checkbox.hpp>
%include <guichan/widgets/container.hpp>
%include <guichan/widgets/scrollarea.hpp>
%include <guichan/widgets/listbox.hpp>
%include <guichan/widgets/dropdown.hpp>
%include <guichan/widgets/icon.hpp>
%include <guichan/widgets/imagebutton.hpp>
%include <guichan/widgets/label.hpp>
%include <guichan/widgets/slider.hpp>
%include <guichan/widgets/radiobutton.hpp>
%include <guichan/widgets/tab.hpp>
%include <guichan/widgets/tabbedarea.hpp>
%include <guichan/widgets/textbox.hpp>
%include <guichan/widgets/textfield.hpp>
%include <guichan/widgets/window.hpp>

