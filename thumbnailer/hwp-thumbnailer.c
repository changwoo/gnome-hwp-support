/* gnome-hwp-support
 * 
 * Copyright (C) 2011-2015 Changwoo Ryu
 * 
 * This program is free software; you can redistribute it and'or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301 USA.
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gsf/gsf.h>


enum {
    FILE_TYPE_AUTO,
    FILE_TYPE_HWP_V5,
    FILE_TYPE_HWPX,
};

int file_type = FILE_TYPE_AUTO;
int max_size = 256;


int
main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "t:s:")) != -1) {
        switch (opt) {
        case 't':
            if (g_ascii_strcasecmp(optarg, "hwpx") == 0)
                file_type = FILE_TYPE_HWPX;
            else if (g_ascii_strcasecmp(optarg, "hwp") == 0)
                file_type = FILE_TYPE_HWP_V5;
            else {
                fprintf(stderr, "Unknown file type: %s", optarg);
                exit(1);
            }
            break;
        case 's':
            max_size = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Unrecognized option %c", opt);
            exit(1);
        }
    }

    if ((argc - optind) < 2) {
        exit(1);
    }

    char *uri = argv[optind];
    char *outfilename = argv[optind + 1];

    gsf_init();

    GError *error = NULL;
    GsfInput *input = gsf_input_gio_new_for_uri(uri, &error);
    if (error) {
        fprintf(stderr, "Can't open input file (%s)\n", error->message);
        exit(1);
    }

    GsfInput *image_child = NULL;
    GsfInfile *infile = NULL;

    // check HWP V5
    if (file_type == FILE_TYPE_AUTO || file_type == FILE_TYPE_HWP_V5) {
        infile = gsf_infile_msole_new(input, &error);
        if (error) {
            if (file_type != FILE_TYPE_AUTO) {
                fprintf(stderr, "Can't open input OLE file (%s)\n", error->message);
                exit(1);
            }
            g_error_free(error);
            error = NULL;
        } else {
            image_child = gsf_infile_child_by_name(infile, "PrvImage");
            g_object_unref(infile);
            if (!image_child) {
                fprintf(stderr, "There is no PrvImage data\n");
                exit(1);
            }
        }
    }

    // check HWPX
    if (image_child == NULL && (file_type == FILE_TYPE_AUTO || file_type == FILE_TYPE_HWPX)) {
        infile = gsf_infile_zip_new(input, &error);
        if (error) {
            if (file_type != FILE_TYPE_AUTO) {
                fprintf(stderr, "Can't open input ZIP file (%s)\n", error->message);
                exit(1);
            }
            g_error_free(error);
            error = NULL;
        } else {
            g_object_unref(input);

            image_child = gsf_infile_child_by_vname(infile, "Preview", "PrvImage.png", NULL);
            g_object_unref(infile);
            if (!image_child) {
                fprintf(stderr, "There is no PrvImage data\n");
                exit(1);
            }
        }
    }

    if (file_type == FILE_TYPE_AUTO && image_child == NULL) {
        fprintf(stderr, "Can't read open input file\n");
        exit(1);
    }

    int size = gsf_input_size(image_child);

    unsigned char *buf;
    buf = g_malloc(size);

    gsf_input_read(image_child, size, buf);
    g_object_unref(image_child);

    GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
    gdk_pixbuf_loader_write(loader, buf, size, &error);
    g_free(buf);
    if (error) {
        fprintf(stderr, "Can't parse image data (%s)\n", error->message);
        exit(1);
    }

    gdk_pixbuf_loader_close(loader, &error);
    if (error) {
        fprintf(stderr, "Can't close pixbuf loader (%s)\n", error->message);
        exit(1);
    }

    GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
    if (! pixbuf) {
        fprintf(stderr, "Can't parse image data: gdk_pixbuf_loader_get_pixbuf() failed\n");
        exit(1);
    }

    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    if (width > max_size || height > max_size) {
        int dwidth, dheight;
        GdkPixbuf *new_pixbuf;
        if (width > max_size) {
            dwidth = max_size;
            dheight = max_size * height / width;
        }
        if (height > max_size) {
            dheight = max_size;
            dwidth = max_size * width / height;
        }

        new_pixbuf = gdk_pixbuf_scale_simple(pixbuf, dwidth, dheight, GDK_INTERP_BILINEAR);
        if (new_pixbuf) {
            g_object_unref(pixbuf);
            pixbuf = new_pixbuf;
        }
    }

    gdk_pixbuf_save(pixbuf, outfilename, "png", &error, NULL);
    g_object_unref(pixbuf);
    if (error) {
        fprintf(stderr, "Can't save image (%s)\n", error->message);
        exit(1);
    }

    gsf_shutdown();

    exit(0);
}
