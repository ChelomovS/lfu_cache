import random

tests_number = 10

for test_number in range(0, tests_number):
    name_of_file = "test_" + f'{test_number + 1:02}' + ".in"
    file = open(name_of_file, 'w')

    cache_capacity  = random.randint(1000, 2000)

    elems_number = random.randint(100000, 200000)

    test_text = str(cache_capacity) + " " + str(elems_number) + "\n"

    for elem_number in range(elems_number) :
        test_text += str(random.randint(0, 100000)) + " "

    file.write(test_text)
    file.close()