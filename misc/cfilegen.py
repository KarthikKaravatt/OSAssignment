import random
f = open("c_file", "w");
num = 1
num2 = 0
types = "WDI"
while num != 101:
    num2 = random.randint(0,2)
    f.write(str(num) + " " + types[num2] + "\n")
    num+=1
