#include "TemperatureReader.h"

TemperatureReader::TemperatureReader() {
    setup();
}

void TemperatureReader::setup() {
    //Characterize ADC at particular atten
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_0db, ADC_WIDTH_BIT_12, 3300, adc_chars);

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_0db);
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_0db);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_0db);

    Homie.getLogger() << "coeff_a: " << adc_chars->coeff_a << endl;
    Homie.getLogger() << "coeff_b: " << adc_chars->coeff_b << endl;
    Homie.getLogger() << "v_ref: " << adc_chars->vref << endl;
    
    //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        Homie.getLogger() << "eFuse Vref" << endl;
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        Homie.getLogger() << "Two Point" << endl;
    } else {
        Homie.getLogger() << "Default" << endl;
    }    

    afFilter = new FilterOnePole(LOWPASS, LOW_PASS_FREQUENCY);
    ruefFilter = new FilterOnePole(LOWPASS, LOW_PASS_FREQUENCY);
    vfFilter = new FilterOnePole(LOWPASS, LOW_PASS_FREQUENCY);
}

void TemperatureReader::readTemperatures() {
    uint32_t afVoltage = readVoltage(ADC1_CHANNEL_6);
    afFilter->input(afVoltage);

    uint32_t ruefVoltage = readVoltage(ADC1_CHANNEL_7);
    ruefFilter->input(ruefVoltage);

    uint32_t vfVoltage = readVoltage(ADC1_CHANNEL_4);
    vfFilter->input(vfVoltage);
}

uint32_t TemperatureReader::readVoltage(adc1_channel_t channel) {
    int adc = adc1_get_raw(channel);
    return esp_adc_cal_raw_to_voltage(adc, adc_chars);
}

uint32_t TemperatureReader::calculateResistence(uint32_t voltage) {
    return voltage * REF_RESISTOR / (REF_VOLTAGE - voltage);
}

double TemperatureReader::calculateTemperature(uint32_t resistence, double offset, double factor) {
    return (resistence - offset) / factor;
}

double TemperatureReader::getOutsideTemperature() {
    float afVoltage = afFilter->output();
    uint32_t afResistence = calculateResistence(afVoltage);
    double afTemperature = calculateTemperature(afResistence, OUTSIDE_TEMP_OFFSET, OUTSIDE_TEMP_FACTOR);
        char logData[100] = {0};
        sprintf(logData, "outside: voltage:%.1f, resistence:%d, temperature:%.1f\n", afVoltage, afResistence, afTemperature);
        Homie.getLogger() << logData;
    return afTemperature;
}

double TemperatureReader::getFlowTemperature() {
    float vfVoltage = vfFilter->output();
    uint32_t vfResistence = calculateResistence(vfVoltage);
    double vfTemperature = calculateTemperature(vfResistence, OUTSIDE_TEMP_OFFSET, OUTSIDE_TEMP_FACTOR);
        char logData[100] = {0};
        sprintf(logData, "flow: voltage:%.1f, resistence:%d, temperature:%.1f\n", vfVoltage, vfResistence, vfTemperature);
        Homie.getLogger() << logData;
    return vfTemperature;
}

double TemperatureReader::getReturnTemperature() {
    float ruefVoltage = ruefFilter->output();
    uint32_t ruefResistence = calculateResistence(ruefVoltage);
    double ruefTemperature = calculateTemperature(ruefResistence, OUTSIDE_TEMP_OFFSET, OUTSIDE_TEMP_FACTOR);
        char logData[100] = {0};
        sprintf(logData, "return: voltage:%.1f, resistence:%d, temperature:%.1f\n", ruefVoltage, ruefResistence, ruefTemperature);
        Homie.getLogger() << logData;
    return ruefTemperature;
}
