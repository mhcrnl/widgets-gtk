#include<gtk/gtk.h>
#include"floatBorder.h"




int main(int argc,char**argv)
{

    gtk_init(&argc,&argv);




    GtkWidget*win=gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_widget_set_size_request(win,400,400);

    GtkWidget*button0=gtk_button_new_with_label("BuTToN0");
    GtkWidget*button1=gtk_button_new_with_label("BuTToN11");

    GtkWidget*fb=(GtkWidget*)g_object_new(TYPE_FLOAT_BORDER,NULL);

    gtk_container_add(GTK_CONTAINER(win),fb);

    float_border_put(FLOAT_BORDER(fb),button0,100,100);
    float_border_put_with_size(FLOAT_BORDER(fb),button1,150,300,200,200);


    gtk_widget_show_all(win);





    gtk_main();
    
    return 0;




}
