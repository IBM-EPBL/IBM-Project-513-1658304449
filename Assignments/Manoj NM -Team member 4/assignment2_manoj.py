import random
import time

i = 0
while (i < 100):
    temp = random.randrange(0, 70)
    mois = random.randrange(0, 150)
    print( "celcius:",temp, "moisture:" ,mois )
    if (temp > 52):
        print("   High Temperature")
    elif (temp <= 52):
        print("   Low Temperature")

    if (mois > 90):
        print("   Very Humid Weather")
    elif (mois <= 90):
        print("   Very Dry Weather")

    i += 1
    time.sleep(3)