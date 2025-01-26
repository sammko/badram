// SPDX-License-Identifier: BSD-2-Clause-Patent
/*
 * Copyright 2012 <James.Bottomley@HansenPartnership.com>
 * Code Copyright 2012 Red Hat, Inc <mjg@redhat.com>
 */
/*
 * This file is an excerpt https://github.com/rhboot/shim/blob/main/lib/execute.c
 */
#include <efi/efi.h>
#include <efi/efilib.h>

EFI_STATUS
generate_path(CHAR16 *name, EFI_LOADED_IMAGE *li, EFI_DEVICE_PATH **path,
              CHAR16 **PathName) {
    unsigned int pathlen;
    EFI_STATUS efi_status = EFI_SUCCESS;
    CHAR16 *devpathstr = DevicePathToStr(li->FilePath), *found = NULL;
    unsigned int i;

    for (i = 0; i < StrLen(devpathstr); i++) {
        if (devpathstr[i] == '/')
            devpathstr[i] = '\\';
        if (devpathstr[i] == '\\')
            found = &devpathstr[i];
    }
    if (!found) {
        pathlen = 0;
    } else {
        while (*(found - 1) == '\\')
            --found;
        *found = '\0';
        pathlen = StrLen(devpathstr);
    }

    if (name[0] != '\\')
        pathlen++;

    *PathName = AllocatePool((pathlen + 1 + StrLen(name)) * sizeof(CHAR16));

    if (!*PathName) {
        Print(L"Failed to allocate path buffer\n");
        efi_status = EFI_OUT_OF_RESOURCES;
        goto error;
    }

    StrCpy(*PathName, devpathstr);

    if (name[0] != '\\')
        StrCat(*PathName, L"\\");
    StrCat(*PathName, name);

    *path = FileDevicePath(li->DeviceHandle, *PathName);

error:
    FreePool(devpathstr);

    return efi_status;
}
