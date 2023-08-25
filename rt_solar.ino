#include <Arduino.h>

class Pump {
  private:
    int _pin;
    int _on_state;
  public:
    Pump(int pin, int on_state) {
        _pin = pin;
        _on_state = on_state;
    }
    
    void begin() {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, _on_state);
    }
    
    void turn_on() {
        digitalWrite(_pin, _on_state);
    }
    
    void turn_off() {
        digitalWrite(_pin, !_on_state);
    }
    
};

class NTCSens {
  private:
    int _pin;
    int _v_div_res_value;
  public:
    NTCSens(int pin, int v_div_res_value) {
        _pin = pin;
        _v_div_res_value = v_div_res_value;
    }
    
    void begin() {
        pinMode(_pin, INPUT);
    }
    
    float read() {
        int adc_val = analogRead(_pin); // 0 - 4095
        double adc_voltage = (adc_val * 3.3) / 4095;
        double ntc_resistance = (adc_voltage * _v_div_res_value) / (3.3 - adc_voltage);
        double temperature = ntc_resistance;
        return temperature;
    }
};

class Switch {
  private:
    int _pin;
    int _on_state;
    bool _pull_up;
  public:
    Switch(int pin, int on_state, bool pull_up=false) {
        _pin = pin;
        _on_state = on_state;
    }
    
    void begin() {
        if (_pull_up)
            pinMode(_pin, INPUT);
        else
            pinMode(_pin, INPUT_PULLUP);
    }
    
    bool is_on() {
        return digitalRead(_pin) == _on_state;
    }
    
    bool is_off() {
        return !is_on();
    }
};

class Heater {
  private:
    int _pin;
    int _on_state;
    
  public:
    Heater(int pin, int on_state) {
        _pin = pin;
        _on_state = on_state;
    }
    
    void turn_on() {
        digitalWrite(_pin, _on_state);
    }
    
    void turn_off() {
        digitalWrite(_pin, !_on_state);
    }
    
    void begin() {
        pinMode(_pin, OUTPUT);
        turn_off();
    }
};

// #define TEMP_SOLAR_MAX
#define TEMP_BOILER_MAX (50)

#define TEMP_DIFF_START (5)
#define TEMP_DIFF_STOP (2)

Pump pump(2, HIGH); // (pin, aktivno nivo)

NTCSens T_solar_sens(25, 1000); // (pin, vrednost na otpornik)
NTCSens T_boiler_sens(26, 1000); // (pin, vrednost na otpornik)

Switch heater_manual_sw(27, LOW, true); // (pin, aktivno nivo, pullup)
Heater heater(4, HIGH);

void setup() {
    pump.begin();
    
    T_solar_sens.begin();
    T_boiler_sens.begin();
    
    heater_manual_sw.begin();
    heater.begin();
}

void loop() {
    if (T_boiler_sens.read() < TEMP_BOILER_MAX) {
        if (heater_manual_sw.is_on()) {
            heater.turn_on();
        }
        
        if (T_solar_sens.read() - T_boiler_sens.read() > TEMP_DIFF_START) {
            pump.turn_on();
        }
        if (T_solar_sens.read() - T_boiler_sens.read() < TEMP_DIFF_STOP) {
            pump.turn_off();
        }
    }
    else {
        heater.turn_off();
    }
    delay(100);
}