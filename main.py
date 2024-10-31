# OOP = Obiektno orientiravnoe programing


types_python = 1, 's' , 2.3 , True, {},[], ()

print(type('str'))

# def a(b):
#     print(b)
#
# a(12242312)

class Car:

    # make = "Audi"

    def drive(self):
        print("car is driving" , self.model)

    def __str__(self):
        return f"""Model {self.model},
age {self.age}"""


    def __init__(self, model, volume, age ):
        self.model = model
        self.volume = volume
        self.age = age

    def start(self):
        print("Car is started in ", self.age)

#Eksemplyar of class
audi = Car("A8", 12.3 , 132)
mazda = Car("sigma" ,  2.4, 12)

mazda.drive()
audi.drive()

print(audi)

#CamelCase = snake_case

