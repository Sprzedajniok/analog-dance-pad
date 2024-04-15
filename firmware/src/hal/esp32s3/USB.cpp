#include <Arduino.h>

#include <USB.h>
#include <USBHID.h>

#include "hal/hal_USB.hpp"
#include "adp_config.hpp"
#include "Reports/Reports.hpp"
#include "Reports/PadReports.hpp"

class USBPad: public USBHIDDevice {
public:
    USBPad();
    void begin();
    void end();

    void sendInputReport();

    // internal use
    uint16_t _onGetDescriptor(uint8_t* buffer);
    virtual uint16_t _onGetFeature(uint8_t report_id, uint8_t* buffer, uint16_t len) override;
    virtual void _onSetFeature(uint8_t report_id, const uint8_t* buffer, uint16_t len) override;
    virtual void _onOutput(uint8_t report_id, const uint8_t* buffer, uint16_t len) override;

private:
    USBHID hid;
};

USBPad::USBPad(): hid()
{
    
}

void USBPad::begin()
{
    static bool initialized = false;
    if(!initialized){
        initialized = true;
        hid.addDevice(this, USB_DescriptorSize());
    }
    
    hid.begin();
    USB.begin();
}

void USBPad::end(){
}

uint16_t USBPad::_onGetDescriptor(uint8_t* buffer)
{
    return USB_WriteDescriptor(buffer);
}

USBPad usbPad;

void HAL_USB_Setup(){
    USB.VID(0x1209);
    USB.PID(0xB196);
    USB.productName("FSR Mini pad");
    USB.manufacturerName("DDR-EXP");

    usbPad.begin();
}

void HAL_USB_Update()
{
    usbPad.sendInputReport();
}

void USBPad::sendInputReport()
{
    InputHIDReport report;
    this->hid.SendReport(INPUT_REPORT_ID, &report, sizeof(report), 200U);
}

uint16_t USBPad::_onGetFeature(uint8_t report_id, uint8_t* buffer, uint16_t len)
{
    bool result = USB_CreateReport(report_id, buffer, &len);
    if(result) {
        return len;
    }
    return 0;
}

void USBPad::_onSetFeature(uint8_t report_id, const uint8_t* buffer, uint16_t len)
{
    USB_ProcessReport(report_id, buffer, len);
}

void USBPad::_onOutput(uint8_t report_id, const uint8_t* buffer, uint16_t len)
{
    USB_ProcessReport(report_id, buffer, len);
}