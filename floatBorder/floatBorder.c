
#include"floatBorder.h"




#define  X 0
#define  Y 1

#define HWND_THICKNESS 6


enum{
    CHILD_PROP_0,
    CHILD_PROP_X,
    CHILD_PROP_Y,
    CHILD_PROP_WIDTH,
    CHILD_PROP_HEIGHT,


    CHILD_PROP_HANDLE_MOVE,//TOP_LEFT,
    CHILD_PROP_HANDLE_RIGHT,
//    CHILD_PROP_HANDLE_LEFT,
    CHILD_PROP_HANDLE_BOTTOM,
//    CHILD_PROP_HANDLE_TOP,
    CHILD_PROP_HANDLE_RESIZE,//BOTTOM_RIGHT,
//    CHILD_PROP_HANDLE_BOTTOM_LEFT,
//    CHILD_PROP_HANDLE_TOP_RIGHT,

    CHILD_PROP_ACTIVED,

    CHILD_PROP_CANBE_OVERLAPPED,
    N_CHILD_PROPS

};




enum{
PROP_0,
PROP_FROZEN,
PROP_MAYBE_OVERLAPPED,
N_PROPS


};

enum{
HWND_MOVE,//TOP_LEFT,
HWND_RIGHT,
HWND_BOTTOM,
HWND_RESIZE,//BOTTOM_RIGHT,

N_HWNDS

};



enum{
RESIZE_CORNER,
MOVE_CORNER

};



typedef struct floatBorderChild FloatBorderChild;


struct floatBorderChild{

    GtkWidget*widget;
    GdkWindow*hwnds[N_HWNDS];

    gint x;
    gint y;
    gint width;
    gint height;

    GtkRequisition min_siz;
    gboolean hwnded[N_HWNDS];
    GdkRectangle position[2];
    int drag_position[2];

    guint actived:1;

    guint canbe_overlapped:1;
    guint canbe_shrink:1;

    guint overlaped:1;
    guint shrink:1;
    guint in_drag:1;


};




struct floatBorderPriv{

GList *children;
guint maybe_overlapped:1;
guint maybe_shrink:1;

gboolean hwnd_mask[N_HWNDS];

guint frozen:1;
guint is_dragging:1;

GHashTable* widget2child;
GHashTable* window2child;


};



/* GObject */
static void float_border_set_property(GObject*object,guint prop_id,const GValue*value,GParamSpec*psepc);
static void float_border_get_property(GObject*object,guint prop_id, GValue*value,GParamSpec*psepc);



/* GtkWidget */

static void float_border_get_preferred_width(GtkWidget* widget,gint *minimum,gint *natural);
static void float_border_get_preferred_height(GtkWidget* widget,gint *minimum,gint *natural);

static void fb_get_size_for_size(GtkWidget*widget,GtkOrientation orientation,gint size,
        GtkRequisition* minimum_req,GtkRequisition* natural_req);

static void float_border_get_preferred_width_for_height(GtkWidget* widget,gint height,gint *minimum,gint *natural);
static void float_border_get_preferred_height_for_width(GtkWidget* widget,gint width,gint *minimum,gint *natural);


static void float_border_realize(GtkWidget*widget);
static void float_border_unrealize(GtkWidget*widget);

static void float_border_map(GtkWidget*widget);
static void float_border_unmap(GtkWidget*widget);


static void float_border_size_allocate(GtkWidget*widget, GtkAllocation *allocation);

static gboolean float_border_draw(GtkWidget*widget,cairo_t*cr);

static gboolean float_border_button_press(GtkWidget*widget,GdkEventButton*e);
static gboolean float_border_button_release(GtkWidget*widget,GdkEventButton*e);
static gboolean float_border_motion_notify(GtkWidget*widget,GdkEventMotion*e);






/* GtkConainer */
static GType float_border_child_type(GtkContainer*container);

static void _float_border_add(GtkContainer*container,GtkWidget*widget);
static void _float_border_remove(GtkContainer*container,GtkWidget*widget);
static void _float_border_forall(GtkContainer*container, gboolean include_internals,GtkCallback callback,gpointer callback_data);

static void float_border_set_child_property(GtkContainer*container,GtkWidget*widget,
        guint prop_id,const GValue*value,GParamSpec*pspec);
static void float_border_get_child_property(GtkContainer*container,GtkWidget*widget,
        guint prop_id, GValue*value,GParamSpec*pspec);

//static GtkWidgetPath* float_border_get_path_for_child(GtkContainer*container,GtkWidget*widget);




/* FloatBorder */

void float_border_put(FloatBorder*fb,GtkWidget*w,int x,int y);
void float_border_remove(FloatBorder*fb,GtkWidget*w);

void float_border_adjust(FloatBorder*fb,GtkWidget*w,int x,int y);






/**/
G_DEFINE_TYPE(FloatBorder,float_border,GTK_TYPE_CONTAINER);




static void float_border_class_init(FloatBorderClass *klass)
{

    GObjectClass*oclass=G_OBJECT_CLASS(klass);
    GtkWidgetClass*wclass= GTK_WIDGET_CLASS(klass);
    GtkContainerClass*cclass= GTK_CONTAINER_CLASS(klass);

    oclass->set_property=float_border_set_property;
    oclass->get_property=float_border_get_property;

//    wclass->draw=float_border_draw;
    wclass->size_allocate=float_border_size_allocate;
    wclass->get_preferred_width=float_border_get_preferred_width;
    wclass->get_preferred_height=float_border_get_preferred_height;
    wclass->realize=float_border_realize;
    wclass->unrealize=float_border_unrealize;
    wclass->map=float_border_map;
    wclass->unmap=float_border_unmap;

    wclass->button_press_event=float_border_button_press;
    wclass->button_release_event=float_border_button_release;
    wclass->motion_notify_event=float_border_motion_notify;


    cclass->add=_float_border_add;
    cclass->remove=_float_border_remove;
    cclass->forall=_float_border_forall;
    cclass->child_type=float_border_child_type;
    cclass->set_child_property=float_border_set_child_property;
    cclass->get_child_property=float_border_get_child_property;

    gtk_container_class_handle_border_width(cclass);

//    g_object_class_override_property (oclass, PROP_ORIENTATION, "orientation");


    g_object_class_install_property(oclass,PROP_FROZEN,
            g_param_spec_boolean("frozen","Frozen","Frozen all widgets within FloatBorder,enfore them can not resize",
                FALSE,G_PARAM_READWRITE));

    g_object_class_install_property(oclass,PROP_MAYBE_OVERLAPPED,
            g_param_spec_boolean("maybe-overlapped","Maybe Overlapped","ensure whether any widget can be overlapped by another widget within FloatBorder",
                FALSE,G_PARAM_READWRITE));



gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_X,
                                  g_param_spec_int ("x","X","x-axis position of current widget.",
                                  0,G_MAXINT,0,
                                  G_PARAM_READWRITE));

gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_Y,
                                  g_param_spec_int("y","Y","y-axis position of current widget.",
                                  0,G_MAXINT,0,
                                  G_PARAM_READWRITE));
gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_WIDTH,
                                  g_param_spec_int ("width","Width","width of current widget.",
                                  0,G_MAXINT,0,
                                  G_PARAM_READWRITE));
gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_HEIGHT,
                                  g_param_spec_int ("height","Height","height of current widget.",
                                  0,G_MAXINT,0,
                                  G_PARAM_READWRITE));


gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_ACTIVED,
                                  g_param_spec_boolean ("actived","Actived","Whether a widget has any resize handle.",
                                  FALSE,
                                  G_PARAM_READWRITE));


gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_CANBE_OVERLAPPED,
                                  g_param_spec_boolean ("canbe-overlapped","Canbe Overlapped","Whether current widget can be overlapped by another.",
                                  FALSE,
                                  G_PARAM_READWRITE));


gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_HANDLE_MOVE,
                                  g_param_spec_boolean ("move-hwnd","Move handle","Whether current widget wrapped by a move handle.",
                                  FALSE,
                                  G_PARAM_READWRITE));

gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_HANDLE_MOVE,
                                  g_param_spec_boolean ("resize-hwnd","resize handle","Whether current widget wrapped by a resize(bottom right) handle.",
                                  FALSE,
                                  G_PARAM_READWRITE));

gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_HANDLE_MOVE,
                                  g_param_spec_boolean ("right-hwnd","Right handle","Whether current widget wrapped by a right handle.",
                                  FALSE,
                                  G_PARAM_READWRITE));

gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_HANDLE_MOVE,
                                  g_param_spec_boolean ("bottom-hwnd","Bottom handle","Whether current widget wrapped by a bottom handle.",
                                  FALSE,
                                  G_PARAM_READWRITE));




g_type_class_add_private(klass,sizeof(FloatBorderPriv));





}




static gint compare_pointer(gconstpointer a,gconstpointer b)
{

return (int)a <(int)b;


}




static void float_border_init(FloatBorder* fb)
{
FloatBorderPriv* priv=FLOAT_BODER_GET_PRIV(fb);
fb->priv=priv;

GtkWidget*widget=GTK_WIDGET(fb);

gtk_widget_set_has_window(widget,FALSE);

//priv->actived=TRUE;
priv->children=NULL;
priv->maybe_overlapped=TRUE;
priv->maybe_shrink=TRUE;
priv->hwnd_mask[HWND_RESIZE]=TRUE;
priv->hwnd_mask[HWND_RIGHT]=TRUE;
priv->hwnd_mask[HWND_BOTTOM]=TRUE;
priv->hwnd_mask[HWND_MOVE]=TRUE;

priv->frozen=FALSE;

priv->widget2child=g_hash_table_new(g_direct_hash,g_direct_equal);
priv->window2child=g_hash_table_new(g_direct_hash,g_direct_equal);
//priv->widget2child=g_tree_new(compare_pointer);//g_hash_table_new(g_direct_hash,g_direct_equal);
//priv->window2child=g_tree_new(compare_pointer);



}



void print_child(FloatBorderChild*fbc,const char*info)
{

    g_print("\n====%s====\n",info);
    g_print("child. widget:%x\n",fbc->widget);
    g_print("child. x:%d\n",fbc->x);
    g_print("child. y:%d\n",fbc->y);
    g_print("child. width:%d\n",fbc->width);
    g_print("child. height:%d\n",fbc->height);
    g_print("child. actived:%d\n",fbc->actived);
    g_print("\n----hwnds----\n");
    int i;
    for(i=0;i<N_HWNDS;i++){
    g_print("child. hwnded[%d]=%d\n",i,fbc->hwnded[i]);
    g_print("child. hwnds[%d]=%x\n",i,fbc->hwnds[i]);
    
    }


}





static void float_border_set_property(GObject*object,guint prop_id,const GValue*value,GParamSpec*pspec)
{

    FloatBorder *fb=FLOAT_BORDER(object);
    FloatBorderPriv*priv=fb->priv;

    switch(prop_id){
    
        case PROP_FROZEN:
            priv->frozen=g_value_get_boolean(value);
//            fb_set_frozen(fb,priv->frozen);
            gtk_widget_queue_resize(GTK_WIDGET(fb));
            break;
        case PROP_MAYBE_OVERLAPPED:
            priv->maybe_overlapped=g_value_get_boolean(value);
//            fb_set_maybe_overlapped(fb,priv->maybe_overlapped);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object,prop_id,pspec);
            break;
    
    }

}



static void float_border_get_property(GObject*object,guint prop_id,GValue*value,GParamSpec*pspec)
{

    FloatBorder* fb=FLOAT_BORDER(object);
    FloatBorderPriv*priv=fb->priv;

    switch(prop_id){
    
        case PROP_FROZEN:
            g_value_set_boolean(value,priv->frozen);
            break;
        case PROP_MAYBE_OVERLAPPED:
            g_value_set_boolean(value,priv->maybe_overlapped);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object,prop_id,pspec);
            break;
    
    }

}



typedef void (*FBCallBack)(FloatBorderChild*,gpointer);


static void do_for_all_children(FloatBorder*fb,FBCallBack callback, gpointer data)
{

    FloatBorderPriv*priv=FLOAT_BORDER(fb)->priv;

    GList*children=priv->children;

    while(children){
        
        (*callback)(children->data,data);
    
        children=children->next;
    }

}


static FloatBorderChild*find_fb_child(FloatBorder*fb,GtkWidget*w)
{

    FloatBorderPriv*priv=FLOAT_BORDER(fb)->priv;
    FloatBorderChild*fbchild;

    fbchild=g_hash_table_lookup(priv->widget2child,w);
//    fbchild=(FloatBorderChild*)g_tree_lookup(priv->widget2child,w);
    if(fbchild)
        return fbchild;

    g_message("---->widget not cached");
    GList*list;
    for (list=priv->children;list;list=list->next){
    
        fbchild=list->data;
        if(fbchild->widget == w){
//            g_tree_replace(priv->widget2child,w,fbchild);
            g_hash_table_insert(priv->widget2child,w,fbchild);
            return fbchild;
        }
    }
    return NULL;
}

static FloatBorderChild*window_to_fbchild(FloatBorder*fb,GdkWindow*window)
{

    FloatBorderPriv*priv=FLOAT_BORDER(fb)->priv;
    FloatBorderChild*fbchild;
//    g_message("::window to child [%x]",window);

    fbchild=g_hash_table_lookup(priv->window2child,window);
//    fbchild=(FloatBorderChild*)g_tree_lookup(priv->window2child,window);
//    g_critical("fbchild:: = %x",fbchild);
    if(fbchild)
        return fbchild;

    g_message("---->window not cached");
    GList*list;
    for (list=priv->children;list;list=list->next){
    
        fbchild=list->data;
        int i;
        for(i=0;i<N_HWNDS;i++){
            if(!fbchild->hwnded[i])
                continue;
            if(fbchild->hwnds[i] == window){
//                g_tree_replace(priv->window2child,window,fbchild);
            g_hash_table_insert(priv->window2child,window,fbchild);
                return fbchild;
            }
        }
    }
    return NULL;

}




static void
float_border_set_child_property (GtkContainer *container,GtkWidget    *child,
                            guint prop_id,const GValue *value,GParamSpec   *pspec)
{

    FloatBorder*fb=FLOAT_BORDER(container);
    FloatBorderChild*fbchild=find_fb_child(fb,child);

    switch(prop_id)
    {

        case CHILD_PROP_X:
            fbchild->x=g_value_get_int(value);
            break;

        case CHILD_PROP_Y:
            fbchild->y=g_value_get_int(value);
            break;

        case CHILD_PROP_WIDTH:
            fbchild->width=g_value_get_int(value);
            break;

        case CHILD_PROP_HEIGHT:
            fbchild->height=g_value_get_int(value);
            break;


        case CHILD_PROP_HANDLE_MOVE:
            fbchild->hwnded[HWND_MOVE]=g_value_get_boolean(value);
            break;
        case CHILD_PROP_HANDLE_RESIZE:
            fbchild->hwnded[HWND_RESIZE]=g_value_get_boolean(value);
            break;

        case CHILD_PROP_HANDLE_RIGHT:
            fbchild->hwnded[HWND_RIGHT]=g_value_get_boolean(value);
            break;

        case CHILD_PROP_HANDLE_BOTTOM:
            fbchild->hwnded[HWND_BOTTOM]=g_value_get_boolean(value);
            break;

        case CHILD_PROP_ACTIVED:
            fbchild->actived=g_value_get_boolean(value);
            break;

        case CHILD_PROP_CANBE_OVERLAPPED:
            fbchild->canbe_overlapped=g_value_get_boolean(value);
            break;

        default:

            GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID(container,prop_id,pspec);
            break;
    
    }
    
//    gtk_widget_queue_draw()

    if(gtk_widget_get_visible(child)&& gtk_widget_get_visible(GTK_WIDGET(fb)))
        gtk_widget_queue_resize(child);

}



static void
float_border_get_child_property (GtkContainer *container,GtkWidget    *child,
                            guint prop_id,GValue *value,GParamSpec   *pspec)
{


    FloatBorder*fb=FLOAT_BORDER(container);
    FloatBorderChild*fbchild=find_fb_child(fb,child);


    switch(prop_id)
    {

        case CHILD_PROP_X:
            g_value_set_int(value,fbchild->x);
            break;

        case CHILD_PROP_Y:
            g_value_set_int(value,fbchild->y);
            break;

        case CHILD_PROP_WIDTH:
            g_value_set_int(value,fbchild->width);
            break;

        case CHILD_PROP_HEIGHT:
            g_value_set_int(value,fbchild->height);
            break;


        case CHILD_PROP_HANDLE_MOVE:
            g_value_set_boolean(value,fbchild->hwnded[HWND_MOVE]);

            break;
        case CHILD_PROP_HANDLE_RESIZE:
            g_value_set_boolean(value,fbchild->hwnded[HWND_RESIZE]);
            break;

        case CHILD_PROP_HANDLE_RIGHT:
            g_value_set_boolean(value,fbchild->hwnded[HWND_RIGHT]);
            break;

        case CHILD_PROP_HANDLE_BOTTOM:
            g_value_set_boolean(value,fbchild->hwnded[HWND_BOTTOM]);
            break;

        case CHILD_PROP_ACTIVED:
            g_value_set_boolean(value,fbchild->actived);

            break;

        case CHILD_PROP_CANBE_OVERLAPPED:
            g_value_set_boolean(value,fbchild->canbe_overlapped);

            break;

        default:

            GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID(container,prop_id,pspec);
            break;
    
    }


}






static GType float_border_child_type(GtkContainer*container)
{
    return GTK_TYPE_WIDGET;
}



static void _get_next_position(FloatBorder*fb,gint* x,gint* y)
{

    if(x)
        (*x)=0;
    if(y)
        (*y)=0;

}


static void _float_border_add(GtkContainer*container,GtkWidget*widget)
{

    FloatBorder*fb=FLOAT_BORDER(container);
    FloatBorderPriv*priv=fb->priv;

    int ix,iy;
    _get_next_position(fb,&ix,&iy);

    float_border_put(fb,widget,ix,iy);


}


static void _float_border_remove(GtkContainer*container,GtkWidget*widget)
{


    float_border_remove(FLOAT_BORDER(container),widget);
/*
    FloatBorder*fb=FLOAT_BORDER(container);
    FloatBorderPriv*priv=fb->priv;

    FloatBorderChild*fbchild=find_fb_child(fb,widget);
    
    gboolean was_visible=gtk_widget_get_visible(fbchild->widget);

    gtk_widget_unparent(fbchild->widget);
    priv->children=g_list_remove(priv->children,fbchild);
    g_slice_free(FloatBorderChild,fbchild);

    if(was_visible && gtk_widget_get_visible(GTK_WIDGET(fb)))
        gtk_widget_queue_resize(GTK_WIDGET(fb));

*/
}



static void _float_border_forall(GtkContainer*container, gboolean include_internals,GtkCallback callback,gpointer callback_data)
{

    FloatBorder* fb=FLOAT_BORDER(container);
    FloatBorderPriv* priv=fb->priv;

    FloatBorderChild*fbchild;

    GList*list=priv->children;
    while(list){
    fbchild=list->data;
   (*callback)(fbchild->widget,callback_data); 
    list=list->next;
    }

}





/**/

typedef struct size_info{

    int expect_width;
    int expect_height;

}SizeInfo;


static void size_negotiation(FloatBorderChild*fbc,gpointer d){

    SizeInfo*info=(SizeInfo*)d;

    GtkRequisition min;

    GtkRequisition childmin;
if(fbc->width==0 || fbc->height==0){
    gtk_widget_get_preferred_size(fbc->widget,&childmin,NULL);
    fbc->width=childmin.width;
    fbc->height=childmin.height;

    fbc->min_siz.width=childmin.width;
    fbc->min_siz.height=childmin.height;

    fbc->position[RESIZE_CORNER].x=childmin.width;
    fbc->position[RESIZE_CORNER].y=childmin.height;
}
    min.width=fbc->width+fbc->x;
    min.height=fbc->height+fbc->y;



    if(min.width > info->expect_width)
        info->expect_width=min.width;

    if(min.height> info->expect_height)
        info->expect_height=min.height;



}



static void float_border_get_preferred_size(GtkWidget*widget, GtkRequisition *minimum_req,GtkRequisition* natural_req)
{

    FloatBorder*fb=FLOAT_BORDER(widget);


    SizeInfo expect_size={0,};
    do_for_all_children(fb,size_negotiation,&expect_size);

    if(minimum_req){
        minimum_req->width=expect_size.expect_width;
        minimum_req->height=expect_size.expect_height;
    }

    if(natural_req){
        natural_req->width=expect_size.expect_width;
        natural_req->height=expect_size.expect_height;
    }

}





static void float_border_get_preferred_width(GtkWidget* widget,gint *minimum,gint *natural)
{

    GtkRequisition minimum_req;
    GtkRequisition natural_req;

    float_border_get_preferred_size(widget,&minimum_req,&natural_req);

    if(minimum)
        (*minimum)=minimum_req.width;
    if(natural)
        (*natural)=natural_req.width;
}






static void float_border_get_preferred_height(GtkWidget* widget,gint *minimum,gint *natural)
{

    GtkRequisition minimum_req;
    GtkRequisition natural_req;

    float_border_get_preferred_size(widget,&minimum_req,&natural_req);

    if(minimum)
        (*minimum)=minimum_req.height;
    if(natural)
        (*natural)=natural_req.height;

}




/*
static void fb_get_size_for_size(GtkWidget*widget,GtkOrientation orientation,gint size,
        GtkRequisition* minimum_req,GtkRequisition* natural_req);

static void float_border_get_preferred_width_for_height(GtkWidget* widget,gint height,gint *minimum,gint *natural);
static void float_border_get_preferred_height_for_width(GtkWidget* widget,gint width,gint *minimum,gint *natural);
*/


static gboolean float_border_button_press(GtkWidget*widget,GdkEventButton*e)
{

    FloatBorder*fb=FLOAT_BORDER(widget);
    FloatBorderPriv* priv=fb->priv;
    g_print("BEFORE window 2 fbchild\n");

    FloatBorderChild*fbchild=window_to_fbchild(fb,e->window);
    g_print("AFTER window 2 fbchild[%x]\n",fbchild);

    if(e->type==GDK_BUTTON_PRESS&&e->button==GDK_BUTTON_PRIMARY){

        if(e->window == fbchild->hwnds[HWND_RIGHT]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=e->x;
            fbchild->drag_position[Y]=0;
            return TRUE;
        }
    
        if(e->window == fbchild->hwnds[HWND_BOTTOM]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=0;
            fbchild->drag_position[Y]=e->y;
            return TRUE;
        }
     
        if(e->window == fbchild->hwnds[HWND_RESIZE]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=e->x;
            fbchild->drag_position[Y]=e->y;
            return TRUE;
        }
        if(e->window == fbchild->hwnds[HWND_MOVE]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=e->x;
            fbchild->drag_position[Y]=e->y;
            return TRUE;
        }



    }
    return FALSE;

}



static gboolean float_border_button_release(GtkWidget*widget,GdkEventButton*e)
{

    FloatBorder*fb=FLOAT_BORDER(widget);
    FloatBorderPriv* priv=fb->priv;
    FloatBorderChild*fbchild=window_to_fbchild(fb,e->window);

    if(e->type==GDK_BUTTON_RELEASE&&e->button==GDK_BUTTON_PRIMARY){

        if(e->window == fbchild->hwnds[HWND_RIGHT]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_BOTTOM]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_RESIZE]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }
 
        if(e->window == fbchild->hwnds[HWND_MOVE]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }
   
    }
    return FALSE;

}






static gboolean float_border_motion_notify(GtkWidget*widget,GdkEventMotion*e)
{

    g_print(":: Motion Notify(window:%x)\n",e->window);

    FloatBorder*fb=FLOAT_BORDER(widget);
    FloatBorderPriv* priv=fb->priv;
    FloatBorderChild*fbchild=window_to_fbchild(fb,e->window);
    g_print("::: fbchild:=%x\n",fbchild);

    GtkAllocation allocation;

    gtk_widget_get_allocation(widget,&allocation);


    if(priv->is_dragging){

    if(e->window == fbchild->hwnds[HWND_RIGHT]){
        fbchild->position[RESIZE_CORNER].x  += e->x - fbchild->drag_position[X];
        fbchild->position[RESIZE_CORNER].x = CLAMP(fbchild->position[RESIZE_CORNER].x,
                fbchild->min_siz.width,
                allocation.width-HWND_THICKNESS-fbchild->x);
        
//        fbchild->position[]
    }

    if(e->window == fbchild->hwnds[HWND_BOTTOM]){
        fbchild->position[RESIZE_CORNER].y += e->y - fbchild->drag_position[Y];
        fbchild->position[RESIZE_CORNER].y = CLAMP(fbchild->position[RESIZE_CORNER].y,
                fbchild->min_siz.height,
                allocation.height-HWND_THICKNESS-fbchild->y);
//        fbchild->position[]
    }

    if(e->window == fbchild->hwnds[HWND_RESIZE]){
        fbchild->position[RESIZE_CORNER].x  += e->x - fbchild->drag_position[X];
        fbchild->position[RESIZE_CORNER].x = CLAMP(fbchild->position[RESIZE_CORNER].x,
                fbchild->min_siz.width,
                allocation.width-HWND_THICKNESS-fbchild->x);

        fbchild->position[RESIZE_CORNER].y += e->y - fbchild->drag_position[Y];
        fbchild->position[RESIZE_CORNER].y = CLAMP(fbchild->position[RESIZE_CORNER].y,
                fbchild->min_siz.height,
                allocation.height-HWND_THICKNESS-fbchild->y);
//        fbchild->position[]
    }


    if(e->window == fbchild->hwnds[HWND_MOVE]){
        fbchild->x  += e->x - fbchild->drag_position[X];
        fbchild->x = CLAMP(fbchild->x,
                HWND_THICKNESS,
                allocation.width-fbchild->position[RESIZE_CORNER].x-HWND_THICKNESS);

        fbchild->y += e->y - fbchild->drag_position[Y];
        fbchild->y = CLAMP(fbchild->y,
                HWND_THICKNESS,
                allocation.height-fbchild->position[RESIZE_CORNER].y-HWND_THICKNESS);
//        fbchild->position[]
    }


    gtk_widget_queue_resize(widget);

    }

    g_print(":: Motion Notify()\n");

    return TRUE;
}






static void create_hwnds(FloatBorderChild*fbc,gpointer d)
{

    g_print(":: create hwnds \n");

    FloatBorder*fb=FLOAT_BORDER(d);
    FloatBorderPriv*priv=fb->priv;
    GtkWidget*widget=GTK_WIDGET(fb);

    GdkWindow*parent_win;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GtkAllocation allocation;

    if(fbc->actived && !priv->frozen && gtk_widget_is_sensitive(fbc->widget)){
    g_print(":::need  create hwnds \n");
   /* need to create hwnds*/ 
    parent_win=gtk_widget_get_window(widget);
        g_print("window ::%x\n",parent_win);

    gtk_widget_get_allocation(widget,&allocation);

    attributes.window_type=GDK_WINDOW_CHILD;
    attributes.wclass=GDK_INPUT_ONLY;
    attributes.event_mask=gtk_widget_get_events(widget);
    attributes.event_mask|=(GDK_BUTTON_PRESS_MASK|
            GDK_BUTTON_RELEASE_MASK|
            GDK_ENTER_NOTIFY_MASK|
            GDK_LEAVE_NOTIFY_MASK|
            GDK_POINTER_MOTION_MASK);
    attributes_mask=GDK_WA_X|GDK_WA_Y;



    attributes.x=0;//fbc->width+allocation.x;
    attributes.y=0;//allocation.y;
    attributes.width=1;//HWND_THICKNESS;
    attributes.height=1;//fbc->height;

    if(fbc->hwnded[HWND_RIGHT]){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_SB_H_DOUBLE_ARROW);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_RIGHT]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_RIGHT]);
    g_object_unref(attributes.cursor);

    }
 
    if(fbc->hwnded[HWND_BOTTOM]){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_SB_V_DOUBLE_ARROW);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_BOTTOM]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_BOTTOM]);
    g_object_unref(attributes.cursor);

    }
    
 
    if(fbc->hwnded[HWND_RESIZE]){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_BOTTOM_RIGHT_CORNER);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_RESIZE]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_RESIZE]);
    g_object_unref(attributes.cursor);

    }

    if(fbc->hwnded[HWND_MOVE]){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_FLEUR);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_MOVE]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_MOVE]);
    g_object_unref(attributes.cursor);

    }


    }else{
    //need not to create hwnds for it;
   g_print("need not create hwnds...\n") ;
    
    
    }

//    print_child(fbc,"after realize");

    g_print("Create hwnds END:::\n");

}


static void destroy_hwnds(FloatBorderChild*fbc,gpointer d){

 
    FloatBorder*fb=FLOAT_BORDER(d);
    FloatBorderPriv*priv=fb->priv;
    GtkWidget*widget=GTK_WIDGET(fb);

   
    if(fbc->hwnded[HWND_RIGHT]){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_RIGHT]);
        gdk_window_destroy(fbc->hwnds[HWND_RIGHT]);
//        fbc->hwnded[HWND_RIGHT]=FALSE;
    }


    if(fbc->hwnded[HWND_BOTTOM]){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_BOTTOM]);
        gdk_window_destroy(fbc->hwnds[HWND_BOTTOM]);
//        fbc->hwnded[HWND_BOTTOM]=FALSE;
    }

    if(fbc->hwnded[HWND_RESIZE]){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_RESIZE]);
        gdk_window_destroy(fbc->hwnds[HWND_RESIZE]);
    }

    if(fbc->hwnded[HWND_MOVE]){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_MOVE]);
        gdk_window_destroy(fbc->hwnds[HWND_MOVE]);
    }



}



static void map_hwnds(FloatBorderChild*fbc,gpointer d){
    FloatBorder*fb=FLOAT_BORDER(d);
    FloatBorderPriv*priv=fb->priv;
    GtkWidget*widget=GTK_WIDGET(fb);

    if(gtk_widget_get_visible(fbc->widget)&& !priv->frozen){

    if(fbc->hwnded[HWND_RIGHT]){
        gdk_window_show(fbc->hwnds[HWND_RIGHT]);
    }
    if(fbc->hwnded[HWND_BOTTOM]){
        gdk_window_show(fbc->hwnds[HWND_BOTTOM]);
    }
    if(fbc->hwnded[HWND_RESIZE]){
        gdk_window_show(fbc->hwnds[HWND_RESIZE]);
    }
    if(fbc->hwnded[HWND_MOVE]){
        gdk_window_show(fbc->hwnds[HWND_MOVE]);
    }
 
    }
}


static void unmap_hwnds(FloatBorderChild*fbc,gpointer d){
    FloatBorder*fb=FLOAT_BORDER(d);
    FloatBorderPriv*priv=fb->priv;
    GtkWidget*widget=GTK_WIDGET(fb);

    if(!priv->frozen){

    if(fbc->hwnded[HWND_RIGHT]){
        gdk_window_hide(fbc->hwnds[HWND_RIGHT]);
    }
    if(fbc->hwnded[HWND_BOTTOM]){
        gdk_window_hide(fbc->hwnds[HWND_BOTTOM]);
    }
    if(fbc->hwnded[HWND_RESIZE]){
        gdk_window_hide(fbc->hwnds[HWND_RESIZE]);
    }
    if(fbc->hwnded[HWND_MOVE]){
        gdk_window_hide(fbc->hwnds[HWND_MOVE]);
    }

    }

}


static void float_border_realize(GtkWidget*widget)
{
    g_message("::REalize: widget:%x\n",widget);

    GtkAllocation allocation;
    GdkWindow * window;
    GdkWindowAttr attributes;
    gint attributes_mask;

    gtk_widget_set_realized(widget,TRUE);


    if(!gtk_widget_get_has_window(widget)){
        window=gtk_widget_get_parent_window(widget);
        g_print("window ::%x\n",window);
        gtk_widget_set_window(widget,window);
        g_object_ref(window);
//        GTK_WIDGET_CLASS(float_border_parent_class)->realize(widget);

    }else{
        gtk_widget_get_allocation(widget,&allocation);

        attributes.window_type=GDK_WINDOW_CHILD;
        attributes.x=allocation.x;
        attributes.y=allocation.y;
        attributes.width=allocation.width;
        attributes.height=allocation.height;

        attributes.wclass=GDK_INPUT_OUTPUT;
        attributes.visual=gtk_widget_get_visual(widget);
        attributes.event_mask=gtk_widget_get_events(widget)|
            GDK_EXPOSURE_MASK|
            GDK_BUTTON_PRESS_MASK|
            GDK_BUTTON_RELEASE_MASK|
            GDK_POINTER_MOTION_MASK;

        attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_VISUAL;

        window=gdk_window_new(gtk_widget_get_parent_window(widget),&attributes,attributes_mask);
        gtk_widget_set_window(widget,window);
        gtk_widget_register_window(widget,window);
        gtk_style_context_set_background (gtk_widget_get_style_context (widget), window);


    }

        g_print("window ::%x\n",window);

    //create hwnds for every child;

    FloatBorder*fb=FLOAT_BORDER(widget);


    do_for_all_children(fb,create_hwnds,fb);


}




static void float_border_unrealize(GtkWidget*widget)
{

    FloatBorder*fb=FLOAT_BORDER(widget);

    do_for_all_children(fb,destroy_hwnds,fb);

    GTK_WIDGET_CLASS(float_border_parent_class)->unrealize(widget);

    /*
    if(gtk_widget_get_has_window(widget)){
    
        GdkWindow*window=gtk_widget_get_window(widget);
        gtk_widget_unregister_window(widget,window);
        gdk_window_destroy(window);
    }else{
//do not need the next line. see gtkwidget.c for details;
//        GTK_WIDGET_CLASS(float_border_parent_class)->unrealize(widget);
    }
*/






}

static void float_border_map(GtkWidget*widget)
{

    FloatBorder*fb=FLOAT_BORDER(widget);


    GTK_WIDGET_CLASS(float_border_parent_class)->map(widget);

    do_for_all_children(fb,map_hwnds,fb);


}



static void float_border_unmap(GtkWidget*widget)
{


    FloatBorder*fb=FLOAT_BORDER(widget);
    do_for_all_children(fb,unmap_hwnds,fb);

    GTK_WIDGET_CLASS(float_border_parent_class)->unmap(widget);


}



static void child_sizeallocate(FloatBorderChild*fbchild,gpointer d)
{

    g_print(":: child_sizeallocate()\n");

    FloatBorder*fb=FLOAT_BORDER(d);
    FloatBorderPriv*priv=fb->priv;
    GtkWidget*widget=GTK_WIDGET(fb);

    GtkRequisition child_requisition;
    GtkAllocation child_allocation;
    GtkAllocation allocation;

    gtk_widget_get_allocation(widget,&allocation);



    if(!gtk_widget_get_visible(fbchild->widget))
        return;

//    gtk_widget_get_preferred_size(fbchild->widget,&child_requisition,NULL);
    
    child_allocation.x=fbchild->x;
    child_allocation.y=fbchild->y;

    if (!gtk_widget_get_has_window (widget)){
        child_allocation.x += allocation.x;
        child_allocation.y += allocation.y;
    }

    child_allocation.width = fbchild->position[RESIZE_CORNER].x;
    child_allocation.height = fbchild->position[RESIZE_CORNER].y;
    gtk_widget_size_allocate (fbchild->widget, &child_allocation);


/*    size allocation for hanles.*/
    


    if(fbchild->hwnded[HWND_RIGHT]){
        gdk_window_move_resize(fbchild->hwnds[HWND_RIGHT],
                child_allocation.x+fbchild->position[RESIZE_CORNER].x,
                child_allocation.y,
                HWND_THICKNESS,
                child_allocation.height
                );

    }



    if(fbchild->hwnded[HWND_BOTTOM]){
        gdk_window_move_resize(fbchild->hwnds[HWND_BOTTOM],
                child_allocation.x,
                child_allocation.y+fbchild->position[RESIZE_CORNER].y,
                child_allocation.width,
                HWND_THICKNESS
                );

    }

    if(fbchild->hwnded[HWND_RESIZE]){
        gdk_window_move_resize(fbchild->hwnds[HWND_RESIZE],
                child_allocation.x+fbchild->position[RESIZE_CORNER].x,
                child_allocation.y+fbchild->position[RESIZE_CORNER].y,
                HWND_THICKNESS,
                HWND_THICKNESS
                );

    }

    if(fbchild->hwnded[HWND_MOVE]){
        gdk_window_move_resize(fbchild->hwnds[HWND_MOVE],
                child_allocation.x-HWND_THICKNESS,
                child_allocation.y-HWND_THICKNESS,
                HWND_THICKNESS,
                HWND_THICKNESS
                );

    }




}


static void float_border_size_allocate(GtkWidget*widget, GtkAllocation *allocation)
{

    FloatBorder*fb=FLOAT_BORDER(widget);
    FloatBorderPriv* priv = fb->priv;
    
    GtkRequisition child_requisition;
    GtkAllocation child_allocation;

    gtk_widget_set_allocation(widget,allocation);

    if(gtk_widget_get_has_window(widget))
    {
        if(gtk_widget_get_realized(widget)){
        gdk_window_move_resize(gtk_widget_get_window(widget),
                    allocation->x,
                    allocation->y,
                    allocation->width,
                    allocation->height);
        
        }
    
    }

    if(gtk_widget_get_realized(widget))
        do_for_all_children(fb,child_sizeallocate,fb);


}



void float_border_put(FloatBorder*fb,GtkWidget*w,int x,int y)
{

    FloatBorderPriv*priv=fb->priv;
    FloatBorderChild*new_child;



    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_IS_WIDGET(w));

    new_child=g_slice_new0(FloatBorderChild);
    new_child->x=x;
    new_child->y=y;
    new_child->width=0;
    new_child->height=0;
    new_child->actived=TRUE;
    new_child->hwnded[HWND_RIGHT]=TRUE;
    new_child->hwnded[HWND_BOTTOM]=TRUE;
    new_child->hwnded[HWND_RESIZE]=TRUE;
    new_child->hwnded[HWND_MOVE]=TRUE;


    new_child->widget=w;

    gtk_widget_set_parent(w,GTK_WIDGET(fb));
    priv->children=g_list_append(priv->children,new_child);

//    g_print(":::origin new_child:=%x\n",new_child);


}



void float_border_remove(FloatBorder*fb,GtkWidget*w)
{

    FloatBorderPriv*priv=fb->priv;
    FloatBorderChild* child;

    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_IS_WIDGET(w));

    child=find_fb_child(fb,w);
    if(child){
    
        gboolean was_visible=gtk_widget_get_visible(w);
    
        gtk_widget_unparent(w);
        priv->children=g_list_remove(priv->children,child);
//        g_list_free(child);
        g_hash_table_remove(priv->widget2child,w);
        int i;
        for(i=0;i<N_HWNDS;i++){
            if(child->hwnded[i])
                g_hash_table_remove(priv->window2child,child->hwnds[i]);
        }
        g_slice_free(FloatBorderChild,child);

        if(was_visible && gtk_widget_get_visible(GTK_WIDGET(fb)))
            gtk_widget_queue_resize(GTK_WIDGET(fb));
    
    
    }

}



//void float_border_adjust(FloatBorder*fb,GtkWidget*w,int x,int y);








