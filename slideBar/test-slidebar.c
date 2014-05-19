
#include<gtk/gtk.h>

#include"slideBar.h"





static void change_offset(GtkRange*range,SlideBar* bar)
{

    slide_bar_set_offset(bar,gtk_range_get_value(range));

}





int main(int argc,char**argv)
{

    gtk_init(&argc,&argv);


    GtkWidget*win=gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_widget_set_size_request(win,400,350);

    GtkWidget*sb=g_object_new(TYPE_SLIDE_BAR,NULL);
    

    GtkWidget*image=gtk_image_new_from_icon_name("go-down",GTK_ICON_SIZE_BUTTON);
    GtkWidget*button=gtk_button_new_with_label("BuTToN");

    GtkWidget*obox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);

    gtk_box_pack_start(GTK_BOX(obox),image,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(obox),button,FALSE,FALSE,0);

    GtkWidget*label=gtk_label_new("Move Your Pointer to Top Center\n<a button hidden there>");
    gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_CENTER);

    gtk_widget_set_size_request(label,400,300);

    GtkWidget*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,4);

    GtkWidget*scale=gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,0,1,0.05);

    gtk_box_pack_start(GTK_BOX(box),sb,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(box),scale,FALSE,FALSE,0);

//    gtk_container_add(GTK_CONTAINER(box),sb);
//    gtk_container_add(GTK_CONTAINER(box),scale);
    gtk_container_add(GTK_CONTAINER(win),box);

    slide_bar_set_over(SLIDE_BAR(sb),obox);
    slide_bar_set_under(SLIDE_BAR(sb),label);



    g_signal_connect(G_OBJECT(scale),"value-changed",G_CALLBACK(change_offset),sb);



    gtk_widget_show_all(win);

    gtk_main();

    return 0;
}
