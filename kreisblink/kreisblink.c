#include <stdlib.h>
#include <stdio.h>
#include <libvisual/libvisual.h>

#define WIDTH  9
#define HEIGHT 5
#define PIXBUF_SIZE WIDTH * HEIGHT * 3

#define ACTOR "lv_scope"
#define INPUT "alsa"

#define x_exit(msg)                             \
    fprintf (stderr, "Error: %s\n", msg);       \
    exit (1);

void v_render(VisVideo *video, VisBin *bin)
{
    char pixbuf[PIXBUF_SIZE];
    int sum = 0;
    int i;

    /* On depth change */
    if (visual_bin_depth_changed (bin)) {
        if (visual_bin_get_depth (bin) != VISUAL_VIDEO_DEPTH_24BIT) {
            x_exit("Unsupported video depth");
        }
    }

    visual_video_set_buffer (video, pixbuf);
    visual_bin_run (bin);

    fwrite(pixbuf, 1, PIXBUF_SIZE, stdout);
    fflush(stdout);
}

int main(int argc, char * argv[])
{
    /* Init libvisual */

    VisVideoDepth depth;
    VisVideo      *video;
    VisBin        *bin;
    char          *plugin;

    visual_init (&argc, &argv);
    bin = visual_bin_new ();
    depth = visual_video_depth_enum_from_value(24);
    visual_bin_switch_set_style (bin, VISUAL_SWITCH_STYLE_MORPH);
    visual_bin_switch_set_steps (bin, 10);
    visual_bin_set_supported_depth (bin, VISUAL_VIDEO_DEPTH_24BIT);

    if (!(video = visual_video_new ())) {
        x_exit ("Cannot create a video surface");
    }
    if (visual_video_set_depth (video, depth) < 0) {
        x_exit ("Cannot set video depth");
    }

    visual_video_set_dimension (video, WIDTH, HEIGHT);

    if (visual_bin_set_video (bin, video)) {
        x_exit ("Cannot set video");
    }

    visual_bin_connect_by_names (bin, ACTOR, INPUT);

    visual_bin_depth_changed (bin);
    visual_bin_realize (bin);
    visual_bin_sync (bin, 0);

    /* Main loop */

    while (1) {
        v_render(video, bin);
        usleep(100000);
    }

    return 0;
}
