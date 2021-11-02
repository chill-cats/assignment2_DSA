from generator import Generator
import random
import sys

def main():
    gen = Generator(20)
    command = []
    command.extend(gen.get_insert() + gen.get_begin()+gen.get_end() + gen.get_print())
    random.shuffle(command)
    print("\n".join(command))
        

main()