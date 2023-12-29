#include "msgbox.hpp"

#if defined(_MSC_VER)
#   include <windows.h>
#   undef min
#   undef max
#elif defined(__linux__)
#   include <gtk/gtk.h>
#endif

void show_msg_box(const std::string& error_msg) {
    #if defined(_MSC_VER)
    MessageBoxA(nullptr, error_msg.c_str(), nullptr, MB_ICONERROR | MB_OK);
	#elif defined(__linux__)
	GtkWidget* dialog = gtk_message_dialog_new(
		nullptr,
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_OK,
		"%s",
		error_msg.c_str()
	);

	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
    #endif
}    