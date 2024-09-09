import random

count_tests = 10

for test_number in range(0, count_tests):
    name_of_file = "test_" + f'{test_number + 1:02}' + ".in"
    file = open(name_of_file, 'w')

    size_of_cache  = random.randint(10000, 20000)
    count_of_elems = random.randint(100000, 200000)

    test_text = str(size_of_cache) + " " + str(count_of_elems) + "\n"

    for elem_number in range(count_of_elems) :
        test_text += str(random.randint(0, 100000)) + " "

    file.write(test_text)
    file.close()