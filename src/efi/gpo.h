/*
 * Simple UEFI boot loader which executes configured EFI images, where the
 * default entry is selected by a configured pattern (glob) or an on-screen
 * menu.
 *
 * All gummiboot code is LGPL not GPL, to stay out of politics and to give
 * the freedom of copying code from programs to possible future libraries.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Copyright (C) 2013 Joonas Lahtinen <joonas.lahtinen@linux.intel.com>
 *
 * "Everything should be made as simple as possible, but not simpler."
 *   -- Albert Einstein
 */
#include <efi.h>

typedef struct {
        UINTN width;
        UINTN height;
} GpoGfxMode;

#ifdef ENABLE_GPO
EFI_STATUS gpo_graphics_mode(GpoGfxMode *gfxmode);
EFI_STATUS gpo_draw_splash(EFI_FILE *root_dir, CHAR16 *path);
#else
static EFI_STATUS gpo_graphics_mode(GpoGfxMode *gfxmode) {
        (void)gfxmode;
        return EFI_UNSUPPORTED;
}

static EFI_STATUS gpo_draw_splash(EFI_FILE *root_dir, CHAR16 *path) {
        (void)root_dir;
        (void)path;
        return EFI_UNSUPPORTED;
}
#endif
