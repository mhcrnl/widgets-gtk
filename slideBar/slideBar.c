
#include"slideBar.h"







struct _slideBarPriv{

    //has_window
    GdkWindow*window;

    GdkWindow*overwindow;
    GtkWidget*over;
    GtkRequisition overR;


    GdkWindow*underwindow;
    GtkWidget*under;
    GtkRequisition underR;


    int min;
    double offset;
    double goal;
    double stepval;

    int interval;
    gboolean to_show;
    
    int timeoutid;


};





G_DEFINE_TYPE(SlideBar,slide_bar,GTK_TYPE_BOX)



static void slide_bar_class_init(SlideBarClass*klass);
static void slide_bar_init(SlideBar*object);

static void slide_bar_get_preferred_width(GtkWidget*,int*,int*);
static void slide_bar_get_preferred_height(GtkWidget*,int*,int*);
static void slide_bar_realize(GtkWidget*);
static void slide_bar_unrealize(GtkWidget*);
static void slide_bar_map(GtkWidget*);
static void slide_bar_unmap(GtkWidget*);

static gboolean slide_bar_enter(GtkWidget*widget,GdkEventCrossing*e);
static gboolean slide_bar_leave(GtkWidget*widget,GdkEventCrossing*e);


static void slide_bar_size_allocate(GtkWidget*,GtkAllocation*);



static void slide_bar_get_under_geometry(SlideBar*bar,GtkAllocation* allocation);
static void slide_bar_get_over_geometry(SlideBar*bar,GtkAllocation* allocation);

static double current_offset(SlideBar*bar);


static void slide_to_show(SlideBar*bar);
static void slide_to_hide(SlideBar*bar);



static void slide_bar_size_negotiate(GtkWidget*,GtkRequisition*minR,GtkRequisition*natR);




static void set_child(SlideBar*bar,GtkWidget**child,GdkWindow*window,GtkWidget*widget);



static void slide_bar_class_init(SlideBarClass*klass)
{


    GtkWidgetClass* wclass=GTK_WIDGET_CLASS(klass);

    wclass->realize=slide_bar_realize;
    wclass->unrealize=slide_bar_unrealize;
    wclass->map=slide_bar_map;
    wclass->unmap=slide_bar_unmap;
    wclass->enter_notify_event=slide_bar_enter;
    wclass->leave_notify_event=slide_bar_leave;

    wclass->size_allocate=slide_bar_size_allocate;

    wclass->get_preferred_width=slide_bar_get_preferred_width;
    wclass->get_preferred_height=slide_bar_get_preferred_height;

    g_type_class_add_private(klass,sizeof(SlideBarPriv));

}



static void slide_bar_init(SlideBar*object)
{


    SlideBarPriv*priv;
    object->priv=SLIDE_BAR_GET_PRIV(object);

    priv=object->priv;

    GtkWidget*widget=GTK_WIDGET(object);

    priv->overR.width=-1;
    priv->overR.height=-1;
    priv->underR.width=-1;
    priv->underR.height=-1;

//    priv->offset=0.7;
    priv->goal=1.0;
    priv->min=5;

    priv->interval=10;
    priv->timeoutid=0;
    priv->stepval=0.05;

    gtk_widget_set_has_window(widget,TRUE);


}

static void slide_bar_size_negotiate(GtkWidget*widget,GtkRequisition*minR,GtkRequisition*natR)
{

    SlideBarPriv*priv=SLIDE_BAR(widget)->priv;

    if(priv->under){
        gtk_widget_get_preferred_size(priv->under,&priv->underR,NULL);
    }

    if(priv->over){
        gtk_widget_get_preferred_size(priv->over,&priv->overR,NULL);
//        g_message("get preferred_size over");
//        g_print("width:%d   height:%d\n",priv->overR.width,priv->overR.height);
    }
    GtkRequisition reqR;

    if(priv->overR.width>priv->underR.width){
        reqR.width=priv->overR.width;
    }else{
        reqR.width=priv->underR.width;
    }


    if(priv->overR.height>priv->underR.height){
        reqR.height=priv->overR.height;
    }else{
        reqR.height=priv->underR.height;
    }


    if(minR){
        minR->width=reqR.width;
        minR->height=reqR.height;
    }

    if(natR){
        natR->width=reqR.width;
        natR->height=reqR.height;
    
    }


}




static void slide_bar_get_preferred_width(GtkWidget*widget,int*min_width,int*nat_width)
{
//    g_message("Get preferred Width");
    SlideBarPriv*priv=SLIDE_BAR(widget)->priv;

    GtkRequisition minR,natR;

    slide_bar_size_negotiate(widget,&minR,&natR);


    if(min_width)
        *min_width=minR.width;
    if(nat_width)
        *nat_width=minR.width;

}



static void slide_bar_get_preferred_height(GtkWidget*widget,int*min_height,int*nat_height)
{
//    g_message("Get preferred Height");
    SlideBarPriv*priv=SLIDE_BAR(widget)->priv;


    GtkRequisition minR,natR;

    slide_bar_size_negotiate(widget,&minR,&natR);


    if(min_height)
        *min_height=minR.height;
    if(nat_height)
        *nat_height=minR.height;


}



static void slide_bar_realize(GtkWidget*widget)
{

    SlideBar*bar=SLIDE_BAR(widget);
    SlideBarPriv*priv=bar->priv;

    gtk_widget_set_realized(widget,TRUE);




    GtkAllocation allocation;
    GdkWindow*parent;
    GdkWindowAttr attributes;
    guint attributes_mask;


    gtk_widget_get_allocation(widget,&allocation);
    attributes.x=allocation.x;
    attributes.y=allocation.y;
    attributes.width=allocation.width;
    attributes.height=allocation.height;
    attributes.window_type=GDK_WINDOW_CHILD;
    attributes.wclass=GDK_INPUT_OUTPUT;
    attributes.event_mask=gtk_widget_get_events(widget);//GDK_EXPOSURE_MASK;
    attributes.visual=gtk_widget_get_visual(widget);

    attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_VISUAL;


    parent=gtk_widget_get_parent_window(widget);

    priv->window=gdk_window_new(parent,&attributes,attributes_mask);

    gtk_widget_register_window(widget,priv->window);
    gtk_widget_set_window(widget,priv->window);


    //UNDER WINDOW
    parent=priv->window;


    slide_bar_get_under_geometry(bar,&allocation);

    attributes.x=allocation.x;
    attributes.y=allocation.y;
    attributes.width=allocation.width;
    attributes.height=allocation.height;

    priv->underwindow=gdk_window_new(parent,&attributes,attributes_mask);

    gtk_widget_register_window(widget,priv->underwindow);
    if(priv->under){
        gtk_widget_set_parent_window(priv->under,priv->underwindow);
    }

    slide_bar_get_over_geometry(bar,&allocation);

    attributes.x=allocation.x;
    attributes.y=allocation.y;
    attributes.width=allocation.width;
    attributes.height=allocation.height;
    attributes.event_mask|=(GDK_ENTER_NOTIFY_MASK|GDK_LEAVE_NOTIFY_MASK);

    priv->overwindow=gdk_window_new(parent,&attributes,attributes_mask);

    gtk_widget_register_window(widget,priv->overwindow);
    if(priv->over){
        gtk_widget_set_parent_window(priv->over,priv->overwindow);
    }

    



}


static void slide_bar_unrealize(GtkWidget*widget)
{

    SlideBarPriv*priv=SLIDE_BAR(widget)->priv;

    GTK_WIDGET_CLASS(slide_bar_parent_class)->unrealize(widget);

    gtk_widget_unregister_window(widget,priv->underwindow);
    gdk_window_destroy(priv->underwindow);
    priv->underwindow=NULL;
    priv->under=NULL;

    gtk_widget_unregister_window(widget,priv->overwindow);
    gdk_window_destroy(priv->overwindow);
    priv->overwindow=NULL;
    priv->over=NULL;
    


}




static void slide_bar_map(GtkWidget*widget)
{
    SlideBarPriv*priv=SLIDE_BAR(widget)->priv;

    GTK_WIDGET_CLASS(slide_bar_parent_class)->map(widget);

    if(priv->under)
        gdk_window_show(priv->underwindow);
    if(priv->over)
    gdk_window_show(priv->overwindow);




}

static void slide_bar_unmap(GtkWidget*widget)
{

    SlideBarPriv*priv=SLIDE_BAR(widget)->priv;

    if(priv->under)
        gdk_window_hide(priv->underwindow);
    if(priv->over)
        gdk_window_hide(priv->overwindow);


    GTK_WIDGET_CLASS(slide_bar_parent_class)->unmap(widget);

}


static gboolean auto_offset(SlideBar*bar)
{
    gboolean rtval;

    SlideBarPriv*priv=bar->priv;
    double stepval=priv->stepval;
    if(priv->to_show){
        rtval=slide_bar_set_offset(bar,priv->offset+stepval);
    }else{
        rtval=slide_bar_set_offset(bar,priv->offset-stepval);
    }
    
    if(!rtval)
        priv->timeoutid=0;
    return rtval;

}



static gboolean slide_bar_enter(GtkWidget*widget,GdkEventCrossing*e)
{
    SlideBarPriv*priv=SLIDE_BAR(widget)->priv;

    if(e->window==priv->overwindow){
    
    priv->to_show=TRUE;

    if(priv->timeoutid!=0)
        g_source_remove(priv->timeoutid);
//    g_print("offset:%g, goal:%g\n",priv->offset,priv->goal);
   
    if(priv->offset<priv->goal){
        priv->timeoutid=g_timeout_add(priv->interval,(GSourceFunc)auto_offset,SLIDE_BAR(widget));
    }else{
        priv->timeoutid=0;
    }

//        slide_to_show(SLIDE_BAR(widget));
    }

    return TRUE;

}


static gboolean slide_bar_leave(GtkWidget*widget,GdkEventCrossing*e)
{

    SlideBarPriv*priv=SLIDE_BAR(widget)->priv;

    if(e->window==priv->overwindow){
        

    priv->to_show=FALSE;

    if(priv->timeoutid!=0)
        g_source_remove(priv->timeoutid);
//    g_print("offset:%g, goal:%g\n",priv->offset,priv->goal);
   
    if(priv->offset>0){
        priv->timeoutid=g_timeout_add(priv->interval,(GSourceFunc)auto_offset,SLIDE_BAR(widget));
    }else{
        priv->timeoutid=0;
    }

//        slide_to_hide(SLIDE_BAR(widget));
    }

    return TRUE;
}



static void slide_bar_get_under_geometry(SlideBar*bar,GtkAllocation* allocation)
{

    SlideBarPriv*priv=bar->priv;

    GtkAllocation alloc;

    gtk_widget_get_allocation(GTK_WIDGET(bar),&alloc);

    allocation->x=alloc.x;
    allocation->y=alloc.y;
    allocation->width=alloc.width;
    allocation->height=alloc.height;

}

static double current_offset(SlideBar*bar)
{

    return bar->priv->offset;

}

static void slide_bar_get_over_geometry(SlideBar*bar,GtkAllocation* allocation)
{

    SlideBarPriv*priv=bar->priv;
    GtkAllocation overA;
    GtkAllocation alloc;

//    gtk_widget_get_allocation(priv->over,&overA);

//    g_print("GEt OVER GeoMetry\nx:%d,  y:%d\nwidth:%d, height:%d\n",overR.x,overA.y,overA.width,overA.height);

    gtk_widget_get_allocation(GTK_WIDGET(bar),&alloc);


    int min=priv->min;
    int maxoffset=priv->overR.height;
    int goaloffset= -maxoffset*(1-priv->goal);


    allocation->x=(alloc.width-priv->overR.width)/2;
    allocation->y=CLAMP(-maxoffset*(1-current_offset(bar)),-maxoffset+min,goaloffset);
    allocation->width=priv->overR.width;
    allocation->height=priv->overR.height;

}


static void slide_bar_size_allocate(GtkWidget*widget,GtkAllocation*allocation)
{

    SlideBar* bar=SLIDE_BAR(widget);
    SlideBarPriv*priv=bar->priv;


//    g_message("Size Allocate::");
//    g_print("x:%d,  y:%d\nwidth:%d, height:%d\n",allocation->x,allocation->y,allocation->width,allocation->height);


    gtk_widget_set_allocation(widget,allocation);



    GtkAllocation allocO,allocU;


    if(gtk_widget_get_realized(widget)){

    gdk_window_move_resize(priv->window,allocation->x,allocation->y,allocation->width,allocation->height);

        if(priv->under){
        
            slide_bar_get_under_geometry(bar,&allocU);
//    g_print("UNDER\nx:%d,  y:%d\nwidth:%d, height:%d\n",allocU.x,allocU.y,allocU.width,allocU.height);

            gdk_window_move_resize(priv->underwindow,allocU.x,allocU.y,allocU.width,allocU.height);
        }
    
        if(priv->over){
        
            slide_bar_get_over_geometry(bar,&allocO);
//    g_print("OVER\nx:%d,  y:%d\nwidth:%d, height:%d\n",allocO.x,allocO.y,allocO.width,allocO.height);

            gdk_window_move_resize(priv->overwindow,allocO.x,allocO.y,allocO.width,allocO.height);
        }

    }

    
    allocU.x=0;
    allocU.y=0;
    gtk_widget_size_allocate(priv->under,&allocU);

    allocO.x=0;
    allocO.y=0;
    gtk_widget_size_allocate(priv->over,&allocO);


}

static void set_child(SlideBar*bar,GtkWidget**child,GdkWindow*window,GtkWidget*widget)
{

    GtkWidget*old=*child;

    //if widget is *child
    if(old){
        g_object_ref(old);
        gtk_container_remove(GTK_CONTAINER(bar),old);
    }

    *child=widget;
    if(*child){
        gtk_widget_set_parent_window(*child,window);
        gtk_container_add(GTK_CONTAINER(bar),*child);
    }
    if(old){
        g_object_unref(old);
    }

}


void slide_bar_set_under(SlideBar*bar,GtkWidget*w)
{
    SlideBarPriv*priv=bar->priv;

    set_child(bar,&priv->under,priv->underwindow,w);

}


void slide_bar_set_over(SlideBar*bar,GtkWidget*w)
{
    SlideBarPriv*priv=bar->priv;

    set_child(bar,&priv->over,priv->overwindow,w);
}


double slide_bar_get_step(SlideBar*bar)
{
    return 1.0/bar->priv->stepval;

}

void slide_bar_set_step(SlideBar*bar,int nsteps)
{
    bar->priv->stepval=1./nsteps;   

}

gboolean slide_bar_set_offset(SlideBar*bar,double offset)
{
    bar->priv->offset=CLAMP(offset,0,1);
    GtkWidget*widget=GTK_WIDGET(bar);

    GtkAllocation geo;
//    gtk_widget_queue_resize(GTK_WIDGET(bar));
  
    if(gtk_widget_get_realized(widget)){
   
       slide_bar_get_over_geometry(bar,&geo);
        gdk_window_move(bar->priv->overwindow,geo.x,geo.y);
    
    }

    if(offset>1||offset<0)
        return FALSE;
    else
        return TRUE;

}



double slide_bar_get_goal(SlideBar*bar)
{
    g_assert(IS_SLIDE_BAR(bar));
    return bar->priv->goal;
}


void slide_bar_set_goal(SlideBar*bar,double goal)
{
    g_assert(IS_SLIDE_BAR(bar));
    bar->priv->goal=CLAMP(goal,0,1);

}


int slide_bar_get_min(SlideBar*bar)
{
    g_assert(IS_SLIDE_BAR(bar));
    return bar->priv->min;
}

void slide_bar_set_min(SlideBar*bar,int offset)
{
    g_assert(IS_SLIDE_BAR(bar));
    bar->priv->min=offset;
    gtk_widget_queue_resize(GTK_WIDGET(bar));

}


double slide_bar_get_speed(SlideBar*bar)
{
    g_assert(IS_SLIDE_BAR(bar));
    return 10./(bar->priv->interval);

}

void slide_bar_set_speed(SlideBar*bar,double s)
{
    g_assert(IS_SLIDE_BAR(bar));
    s=CLAMP(s,0.1,1.0);

    bar->priv->interval=10./s;

}

GtkWidget* slide_bar_new()
{
return (GtkWidget*)g_object_new(TYPE_SLIDE_BAR,NULL);
}


