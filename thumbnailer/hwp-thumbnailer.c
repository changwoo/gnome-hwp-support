/* gnome-hwp-support
 * 
 * Copyright (C) 2011-2014 Changwoo Ryu
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
/*
 * 본 제품은 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여
 * 개발하였습니다.
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gsf/gsf-utils.h>
#include <gsf/gsf-input-gio.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-infile-msole.h>

int max_size = 256;

int
main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
        case 's':
            max_size = atoi(optarg);
            break;
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
	
    GsfInfile *infile = gsf_infile_msole_new(input, &error);
    if (error) {
        fprintf(stderr, "Can't read MSOLE data (%s)\n", error->message);
        exit(1);
    }
    g_object_unref(input);
    GsfInput *child = gsf_infile_child_by_name(infile, "PrvImage");
    g_object_unref(infile);
    if (!child) {
        fprintf(stderr, "There is no PrvImage data\n");
        exit(1);
    }
    int size = gsf_input_size(child);

    unsigned char *buf;
    buf = g_malloc(size);

    gsf_input_read(child, size, buf);
    g_object_unref(child);

    GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
    gdk_pixbuf_loader_write(loader, buf, size, &error);
    g_free(buf);
    if (error) {
        fprintf(stderr, "Can't parse image data (%s)\n", error->message);
        exit(1);
    }
    
    GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
    if (! pixbuf) {
        fprintf(stderr, "Can't parse image data: gdk_pixbuf_loader_get_pixbuf() failed\n");
        exit(1);
    }
    gdk_pixbuf_loader_close(loader, NULL);

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
