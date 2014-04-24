#ifndef _H_FLOAT_BORDER_
#define _H_FLOAT_BORDER_



#include<gtk/gtk.h>






#define TYPE_FLOAT_BORDER (float_border_get_type())
#define FLOAT_BORDER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),TYPE_FLOAT_BORDER,FloatBorder))
#define FLOAT_BORDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),TYPE_FLOAT_BORDER,FloatBorderClass))

#define IS_FLOAT_BORDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),TYPE_FLOAT_BORDER))
#define IS_FLOAT_BORDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),TYPE_FLOAT_BORDER))

#define FLOAT_BODER_GET_CLASS(obj) (G_TYPE_CHECK_INSTANCE_GET_CLASS((obj),TYPE_FLOAT_BORDER,FloatBorderClass))
#define FLOAT_BODER_GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),TYPE_FLOAT_BORDER,FloatBorderPriv))






typedef struct floatBorderPriv FloatBorderPriv;

typedef struct floatBorder{

    GtkContainer parent;
    FloatBorderPriv*priv;

}FloatBorder;

typedef struct floatBorderClass{

    GtkContainerClass parent_class;

    void (*for_reserved0) (void);
    void (*for_reserved1) (void);


}FloatBorderClass;








GType float_border_get_type(void);




void float_border_put(FloatBorder*fb,GtkWidget*w,int x,int y);
void float_border_put_with_size(FloatBorder*fb,GtkWidget*w,int x,int y,int width,int height);
void float_border_remove(FloatBorder*fb,GtkWidget*w);

void float_border_move(FloatBorder*fb,GtkWidget*w,int x,int y);
void float_border_move_resize(FloatBorder*fb,GtkWidget*w,int x,int y,int width,int height);
void float_border_resize(FloatBorder*fb,GtkWidget*w,int width,int height);

void float_border_reorder(FloatBorder*fb, GtkWidget*w, GtkWidget*sibling, gboolean above);








#endif
