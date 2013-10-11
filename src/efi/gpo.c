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

#include "gpo.h"
#include <efilib.h>

#if 0
static VOID dump_graphics_mode(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info) {
        static const CHAR16* const pixel_format_names[PixelFormatMax] = {
                L"RGBX",
                L"BGRX",
                L"BitMask",
                L"BltOnly"
        };
        Print(L"%d(%d)x%d @ %s", info->HorizontalResolution, info->PixelsPerScanLine, info->VerticalResolution,
              pixel_format_names[info->PixelFormat]);
        Print(L" {%08x,%08x,%08x,%08x}\n", info->PixelInformation.RedMask, info->PixelInformation.GreenMask,
              info->PixelInformation.BlueMask, info->PixelInformation.GreenMask);
}

static EFI_STATUS dump_graphics_modes(VOID) {
        EFI_GUID GraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
        EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput = NULL;
        EFI_STATUS err;
        UINTN i;

        err = LibLocateProtocol(&GraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
        if (EFI_ERROR(err))
                return err;

        Print(L"\nCurrent mode %d (of %d)\n", GraphicsOutput->Mode->Mode, GraphicsOutput->Mode->MaxMode);

        for (i = 0; i < GraphicsOutput->Mode->MaxMode; ++i) {
                EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
                UINTN info_size;
 
                err = uefi_call_wrapper(GraphicsOutput->QueryMode, 4, GraphicsOutput, i, &info_size, &info);
                if (EFI_ERROR(err))
                        return err;

                 Print(L"[%d]", i);
                 dump_graphics_mode(info);
        }

 	return EFI_SUCCESS;
}

#endif // 0

EFI_STATUS gpo_graphics_mode(GpoGfxMode *gfxmode) {
        EFI_GUID GraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
        EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput = NULL;
        EFI_STATUS err;
        UINTN i;

        if (!gfxmode)
                return EFI_SUCCESS;

        err = LibLocateProtocol(&GraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
        if (EFI_ERROR(err))
                return err;

        for (i = 0; i < GraphicsOutput->Mode->MaxMode; ++i) {
                EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
                UINTN info_size;
 
                err = uefi_call_wrapper(GraphicsOutput->QueryMode, 4, GraphicsOutput, i, &info_size, &info);
                if (EFI_ERROR(err))
                        return err;
                if ((info->HorizontalResolution == gfxmode->width) && (info->VerticalResolution == gfxmode->height))
                        break;
        }

        if (i >= GraphicsOutput->Mode->MaxMode)
         	return EFI_NOT_FOUND;

        if (GraphicsOutput->Mode->Mode != i) {
                err = uefi_call_wrapper(GraphicsOutput->SetMode, 2, GraphicsOutput, i);
                if (EFI_ERROR(err))
                        return err;
        }
       	return EFI_SUCCESS;
}

extern UINTN file_read(EFI_FILE_HANDLE, CHAR16 *, CHAR8 **);

EFI_STATUS gpo_draw_splash(EFI_FILE *root_dir, CHAR16 *path) {
        EFI_GUID GraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
        EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput = NULL;
        EFI_STATUS err;
        UINT16 w;
        UINT16 h;
        UINTN dst_x;
        UINTN dst_y;
        UINTN len;
        CHAR8* content;
        
        if (!path || !path[0])
                return EFI_SUCCESS;

        err = LibLocateProtocol(&GraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
        if (EFI_ERROR(err))
                goto err;

        len = file_read(root_dir, path, &content);
        if (len <= 0)
                goto err;

        CHAR8* p;
        p = content;
        if(strncmpa((CHAR8*)"BGRX", p, 4)) {
                Print(L"\nInvalid or corrupt splash file: '%s'!\n", path);
                goto err2;
        }
        p += 4;

        w = (UINT16)*p++;
        w <<= 8;
        w |= (UINT16)*p++;

        h = (UINT16)*p++;
        h <<= 8;
        h |= (UINT16)*p++;
/*        Print(L"Splash size: %dx%d!\n", w, h);*/
        dst_x = (GraphicsOutput->Mode->Info->HorizontalResolution - w)/2;
        dst_y = (GraphicsOutput->Mode->Info->VerticalResolution - h)/2;
        err = uefi_call_wrapper(GraphicsOutput->Blt, 10, GraphicsOutput, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)p,
                                EfiBltBufferToVideo, 0, 0, dst_x, dst_y, w, h, w*4);

        /* FIXME: Artificial stall */
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);

        FreePool(content);
        return err;
err2:
        FreePool(content);
err:
        return EFI_LOAD_ERROR;
}

