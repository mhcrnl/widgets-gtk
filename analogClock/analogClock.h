#ifndef _H_ANALOG_CLOCK_
#define _H_ANALOG_CLOCK_

#include<gtk/gtk.h>


#define TYPE_ANALOG_CLOCK (analog_clock_get_type())
#define ANALOG_CLOCK(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),TYPE_ANALOG_CLOCK,AnalogClock))
#define IS_ANALOG_CLOCK(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),TYPE_ANALOG_CLOCK))

#define ANALOG_CLOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),TYPE_ANALOG_CLOCK,AnalogClockClass))
#define IS_ANALOG_CLOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),TYPE_ANALOG_CLOCK))
#define ANALOG_CLOCK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),TYPE_ANALOG_CLOCK,AnalogClockClass))

#define ANALOG_CLOCK_GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),TYPE_ANALOG_CLOCK,AnalogClockPriv))


typedef struct clockVal
{
//  int h;
//  int m;
//  int s;
  int ts;
  gboolean inited;
} ClockVal;




typedef struct analogClockPriv
{

  int x, y;
  int width, height;
  guint timeoutid;
  const gchar *font;
  gboolean show_sec;
  gboolean show_num;
  gboolean sensitive;
  ClockVal clock_val;

} AnalogClockPriv;

typedef struct analogClock
{

  GtkDrawingArea parent;
  AnalogClockPriv *priv;

} AnalogClock;

typedef struct analogClockClass
{

  GtkDrawingAreaClass parent_class;

} AnalogClockClass;



enum
{
  PROP_0,
  PROP_SHOWSEC,
  PROP_SHOWNUM,
  PROP_SENSITIVE
};


GType analog_clock_get_type (void);

//void analog_clock_set_asctime(AnalogClock*,const char*);
//void analog_clock_set_time(AnalogClock*, int )
void analog_clock_flush (AnalogClock *);

void analog_clock_show_sec (AnalogClock *, gboolean);
void analog_clock_show_num (AnalogClock *, gboolean);
void analog_clock_set_font (AnalogClock *, gchar * desc);
void analog_clock_set_sensitive (AnalogClock *, gboolean);





#endif
