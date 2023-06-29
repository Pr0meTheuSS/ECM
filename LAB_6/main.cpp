#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

void printdev(libusb_device *dev){
    libusb_device_descriptor desc; // дескриптор устройства
    libusb_config_descriptor *config; // дескриптор конфигурации объекта
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0){
        fprintf(stderr,
        "Ошибка: дескриптор устройства не получен, код: %d.\n",r);
        return;
    }
    libusb_device_handle** dev_handle = new libusb_device_handle*;
    r = libusb_open(dev, dev_handle);
    if (r < 0) {
        fprintf(stderr,
        "Ошибка: дескриптор устройства не может быть открыт, код: %d.\n",r);
        return;
    }
    unsigned char serialNumbStr[50] = {0};
    r = libusb_get_string_descriptor_ascii(*dev_handle, desc.iSerialNumber, serialNumbStr, 49);
    if (r < 0) {
        strcat((char*)serialNumbStr, libusb_error_name(r));
    }
    // получить конфигурацию устройства
    libusb_get_config_descriptor(dev, 0, &config);
    printf("%-16x | %-27x | %-24x | %25s\n",
        (int)desc.bDeviceClass,
        (int)desc.idVendor,
        (int)desc.idProduct,
        serialNumbStr 
    );
    libusb_close(*dev_handle);
    libusb_free_config_descriptor(config);
    delete dev_handle;
}

int main(){
    libusb_device** devs; // указатель на указатель на устройство,
    // используется для получения списка устройств
    libusb_context *ctx = NULL; // контекст сессии usb
    int r; // для возвращаемых значений
    ssize_t cnt; // число найденных USB-устройств
    // инициализировать библиотеку usb, открыть сессию работы с usb
    r = libusb_init(&ctx);
    if(r < 0) {
        fprintf(stderr,
        "Ошибка: инициализация не выполнена, код: %d.\n", r);
        return 1;
    }
    // задать уровень подробности отладочных сообщений
    libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL);
    // получить список всех найденных USB- устройств
    cnt = libusb_get_device_list(ctx, &devs);
    if(cnt < 0) {
        fprintf(stderr,
        "Ошибка: список USB устройств не получен.\n");
        return 1;
    }
    printf("найдено устройств: %ld\n", cnt);
    printf("=============================="
    "=============================\n");
    printf("%25s", "класс устройства |");
    printf("%25s", " идентификатор производителя |");
    printf("%25s", " идентификатор устройства |");
    printf("%25s", " серийный номер usb устройства\n");
    for(ssize_t i = 0; i < cnt; i++) { // цикл перебора всех устройств
        printdev(devs[i]); // печать параметров устройства
    }
    printf("=============================="
    "=============================\n");
    // освободить память, выделенную функцией получения списка устройств
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx); // завершить работу с библиотекой usb,
    // закрыть сессию работы с usb
    return 0;
}

