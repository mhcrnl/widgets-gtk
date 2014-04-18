#ifndef _H_DIGITAL_CLOCK_
#define _H_DIGITAL_CLOCK_

#include<gtk/gtk.h>
#include<string.h>


#define TYPE_DIGITAL_CLOCK (digital_clock_get_type())
#define DIGITAL_CLOCK(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),TYPE_DIGITAL_CLOCK,DigitalClock))
#define IS_DIGITAL_CLOCK(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),TYPE_DIGITAL_CLOCK))

#define DIGITAL_CLOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),TYPE_DIGITAL_CLOCK,DigitalClockClass))
#define IS_DIGITAL_CLOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),TYPE_DIGITAL_CLOCK))
#define DIGITAL_CLOCK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),TYPE_DIGITAL_CLOCK,DigitalClockClass))

#define DIGITAL_CLOCK_GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),TYPE_DIGITAL_CLOCK,DigitalClockPriv))

#define LENTIMESTR 13





typedef struct digitalClockPriv {

    int x, y;
    int width, height;
    int fsiz;
    guint timeoutid;
    const gchar *font;
    gchar*time_val;
    gboolean mode12;
    gboolean show_sec;
    gboolean show_msec;
    gboolean blink;
    gboolean sensitive;

} DigitalClockPriv;

typedef struct digitalClock {

    GtkDrawingArea parent;
    DigitalClockPriv *priv;

} DigitalClock;

typedef struct digitalClockClass {

    GtkDrawingAreaClass parent_class;

} DigitalClockClass;



enum {
    PROP_0,
    PROP_FSIZ,
    PROP_FONT,
    PROP_MODE12,
    PROP_SHOWSEC,
    PROP_SHOWMSEC,
    PROP_BLINK,
    PROP_SENSITIVE
};


GType digital_clock_get_type(void);

//void digital_clock_set_asctime(DigitalClock*,const char*);
//void digital_clock_set_time(DigitalClock*, int )
void digital_clock_flush(DigitalClock *);

void digital_clock_show_sec(DigitalClock *, gboolean);
void digital_clock_show_msec(DigitalClock *, gboolean);
void digital_clock_set_blink(DigitalClock *, gboolean);
void digital_clock_set_font(DigitalClock *, gchar * desc);
void digital_clock_set_font_size(DigitalClock * dclock, gint size);
void digital_clock_set_sensitive(DigitalClock *, gboolean);





#endif
