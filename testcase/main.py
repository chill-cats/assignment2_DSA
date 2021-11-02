from gen import Generator
import random
import sys

def main():
    gen = Generator(50)
    command = []
    command.extend(gen.get_insert(random.randint(100,150)+10) + 
                    gen.get_begin(random.randint(5,20))  +
                    gen.get_print(random.randint(20,50) + 2) +
                    gen.get_end(random.randint(5,10) + 1) +
                    gen.get_lookup(random.randint(5,9))
                    )
    for i in range(3):
        random.shuffle(command)
    with open("testcase", mode = 'w', encoding='utf8') as f:
        f.write(command[0])
        for item in command[1:] :
            f.write("\n")
            f.write(item)

    f.close()
main()