#ifndef _H_SLIDE_BAR_
#define _H_SLIDE_BAR_


#include<gtk/gtk.h>


#define TYPE_SLIDE_BAR slide_bar_get_type()

#define SLIDE_BAR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),TYPE_SLIDE_BAR,SlideBar))
#define IS_SLIDE_BAR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),TYPE_SLIDE_BAR))

#define SLIDE_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),TYPE_SLIDE_BAR, SlideBarClass))
#define IS_SLIDE_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),TYPE_SLIDE_BAR))

#define SLIDE_BAR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),TYPE_SLIDE_BAR,SlideBarClass))
#define SLIDE_BAR_GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),TYPE_SLIDE_BAR,SlideBarPriv))


typedef struct _slideBarPriv SlideBarPriv;


typedef struct _slideBar{

    GtkBox parent;
    SlideBarPriv* priv;


}SlideBar;




typedef struct _slideBarClass{

    GtkBoxClass parent_class;



}SlideBarClass;




GType slide_bar_get_type(void);



GtkWidget* slide_bar_new();



void slide_bar_set_under(SlideBar*bar,GtkWidget*w);
void slide_bar_set_over(SlideBar*bar,GtkWidget*w);

gboolean slide_bar_set_offset(SlideBar*bar,double offset);
void slide_bar_set_goal(SlideBar*bar,double goal);
void slide_bar_set_min(SlideBar*bar,int offset);



#endif
