#include <efi/efi.h>
#include <efi/efilib.h>

UINT64 regions[] = {
    // addr, size (pages)
    0x04f950000, 0x10,
    0x297f00000, 0x50,
};

// https://github.com/rhboot/shim/blob/main/lib/execute.c
EFI_STATUS
generate_path(CHAR16 *name, EFI_LOADED_IMAGE *li, EFI_DEVICE_PATH **path,
              CHAR16 **PathName);

EFI_STATUS
run_systemd(EFI_HANDLE image_handle) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE *li;

    Print(L"=> HandleProtocol...\n");

    status = uefi_call_wrapper(
        BS->HandleProtocol, 3, image_handle,
        ((EFI_GUID *)&(const EFI_GUID)EFI_LOADED_IMAGE_PROTOCOL_GUID),
        (void **)&li);

    if (EFI_ERROR(status)) {
        Print(L"Failed to get loaded image protocol: %d\n", status);
        return status;
    }

    Print(L"=> generate_path...\n");

    EFI_DEVICE_PATH *devpath;
    CHAR16 *PathName;

    status = generate_path(L"\\EFI\\systemd\\systemd-bootx64.efi", li, &devpath,
                           &PathName);
    if (EFI_ERROR(status)) {
        Print(L"Failed to generate path: %d\n", status);
        return status;
    }

    Print(L"=> LoadImage...\n");

    EFI_HANDLE h;
    status = uefi_call_wrapper(BS->LoadImage, 6, FALSE, image_handle, devpath,
                               NULL, 0, &h);
    if (EFI_ERROR(status)) {
        Print(L"Failed to load image: %d\n", status);
        goto error;
    }

    Print(L"   Loaded image at %lx (0x%lx bytes)\n", h, li->ImageSize);
    Print(L"=> StartImage...\n");
    status = uefi_call_wrapper(BS->StartImage, 3, h, NULL, NULL);

    uefi_call_wrapper(BS->UnloadImage, 1, h);

error:
    FreePool(PathName);
    FreePool(devpath);

    return status;
}

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *systab) {
    EFI_STATUS status;
    InitializeLib(image_handle, systab);

    for (int i = 0; i < sizeof(regions) / sizeof(regions[0]); i += 2) {
        status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress,
                                   EfiUnusableMemory, regions[i + 1],
                                   &regions[i]);

        if (EFI_ERROR(status)) {
            Print(L"Failed to allocate pages: %d\n", status);
            return status;
        }

        Print(L"%d: Allocated %d pages at %lx (0x%lx bytes)\n",
              i / 2, regions[i + 1], regions[i], regions[i + 1] * 4096);
    }

    return run_systemd(image_handle);
}
