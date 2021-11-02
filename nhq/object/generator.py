import sys
from rstr import xeger as xe
import random

class Identifer:
    def __init__(self, num):
        self.name_reg = (r'([a-z]\w)*')
        self.num = num

    def get_name(self, size, number):
        name_list = []
        for i in range(number):
            name_list.append(xe(self.name_reg)[:size])
        return name_list
    

class Generator(Identifer):
    def __init__(self, num):
        self.num = num
        self.name_reg = (r'([a-z]\w)*')
        self.name_faultreg = (r'(\w)*')
        self.insert_reg = (r'(^INSERT ([a-z]\w*)[:10] (string|number|\((?:|(?:number|string)(?:,(?:number|string))*)\)->(?:number|string)) (true|false)$)')
        #self.assign_reg = (r'(^ASSIGN ([a-z]\w*) (\d+|[\dA-Za-z\s]*|[a-z]\w*|[a-z]\w*\((?:|(?:\d+|[\dA-Za-z\s]*|[a-z]\w*)(?:,(?:\d+|[\dA-Za-z\s]*|[a-z]\w*))*)\))$)')
        self.look_up = (r'^LOOKUP ([a-z]\w)*')
        self.name_list = Identifer.get_name(self,6,20)
    
    def get_insert(self):
        lines = []
        name = self.name_list
        

        for i in range(self.num):
            insert = xe(self.insert_reg)
            tmp = insert.split(" ")
            tmp[1] = random.choice(name)
            lines.append(" ".join(tmp))
        return lines

    def get_lookup(self):
        lines = []
        name = self.name_list

        for i in range(self.num):
            lookup = xe(self.look_up)
            tmp = lookup.split(" ")
            tmp[1] = random.choice(name)
            lines.append(" ".join(tmp))
        return lines
    def get_end(self):
        return ["END" for i in range(3)]

    def get_begin(self):
        return ["BEGIN" for i in range(self.num)]
    
    def get_print(self):
        return ["PRINT" for i in range(self.num)]

    


