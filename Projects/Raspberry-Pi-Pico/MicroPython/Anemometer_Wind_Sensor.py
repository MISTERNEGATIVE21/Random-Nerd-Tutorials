# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-anemomer-micropython/

from machine import ADC, Pin
from time import sleep

# Constants
anemometer_pin = 26    # GPIO pin connected to anemometer
min_voltage = 0.033    # Voltage corresponding to 0 m/s
max_voltage = 3.3      # Voltage corresponding to 32.4 m/s (max speed when using voltage divider)
max_wind_speed = 32.4  # Maximum wind speed in m/s

# Conversion factors
mps_to_kmh = 3.6      # 1 m/s = 3.6 km/h
mps_to_mph = 2.23694  # 1 m/s = 2.23694 mph

# Set up the ADC
adc = ADC(Pin(anemometer_pin))

while True:
    # Read analog value from anemometer
    adc_value = adc.read_u16() # read value, 0-65535 across voltage range 0.0v - 3.3v
    
    # Convert ADC value to voltage
    voltage = (adc_value / 65535.0) * 3.3
     
    # Ensure the voltage is within the anemometer operating range
    if voltage < min_voltage:
        voltage = min_voltage
    elif voltage > max_voltage:
        voltage = max_voltage
    
    # Map the voltage to wind speed
    wind_speed_mps = ((voltage - min_voltage) / (max_voltage - min_voltage)) * max_wind_speed

    # Convert wind speed to km/h and mph
    wind_speed_kmh = wind_speed_mps * mps_to_kmh
    wind_speed_mph = wind_speed_mps * mps_to_mph

    # Print wind speed
    print("Wind Speed:")
    print("{:.2f} m/s".format(wind_speed_mps))
    print("{:.2f} km/h".format(wind_speed_kmh))
    print("{:.2f} mph".format(wind_speed_mph))
    print() 
    
    sleep(1)