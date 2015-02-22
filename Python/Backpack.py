'''
Created on Feb 2, 2015

@author: Kevin
'''

class Item:
    def __init__(self, name, width = 2, height = 3, dataBlob = None, ID = None, x=0,y=0):
        self.width = width
        self.height = height
        self.name = name
        self.x = x
        self.y = y
        self.dataBlob = dataBlob
        if(ID is not None):
            self.id = ID;
        else:
            self.id = hash(name) + hash(dataBlob)

    def __str__(self):
        string = "X: " + str(self.x) + " Y: " + str(self.y)
        string += " Width: " + str(self.width) + " Height: " + str(self.height) 
        string += " Name: " + self.name         
        return string
         
#Used in packing. Tall items are bigger than wide items.         
def cmpItems(a, b):
    if(a.height < b.height):
        return -1;
    elif(a.height > b.height):
        return 1;
    else:
        if(a.width < b.width):
            return -1;
        elif(a.width > b.width):
            return 1;
        else:
            return cmp(a.name, b.name);


            
#This backpack stores items in redundant data structures:
#One is a grid matrix for easy look up based on location
#The other is a dictionary of the items for easy lookup/iteration
class Backpack:
    def __init__(self, width = 10, height = 4):
        self.width = width
        self.height = height
        self.capacity = width * height
        self.items = [ [None]*width for _ in range(height) ]
        self.itemDict = dict()
    
    def __str__(self):
        string = ""
        for item in self.itemDict.itervalues(): string += str(item)        
        return string;
        
    def __insertItem(self, item, x, y):
        for y1 in range(y, y + item.height):
            for x1 in range(x, x + item.width):                    
                self.items[y1][x1] = item
    
    #Make a list of all (x,y) coords. For each item we have to add, search the slots
    #in order. Each time we come across a slot that's already occupied in the real backpack
    #delete the slot. This makes it so we never ignore any spaces, but we only consider a used space once 
    def pack(self):
        for sub_list in self.items:
            for i in range(len(sub_list)):
                sub_list[i] = None
        
        slotsLeft = []
        for y in range(0, self.height):
            for x in range(0, self.width):
                slotsLeft.append((x,y))
        
        toAdd = self.itemDict.values()
        toAdd.sort(cmpItems)
        toRemove = []
        
        while(len(toAdd) > 0):
            item = toAdd.pop()
            toRemove[:] = []
            for slot in slotsLeft:
                x = slot[0]; y = slot[1]
                if(self.items[y][x] is None):
                    if(self.itemFits(item, x, y)):
                        item.x = x; item.y = y;
                        self.__insertItem(item, x, y)
                        toRemove.append(slot)
                        break;
                else:
                    toRemove.append(slot)
            for i in toRemove: slotsLeft.remove(i)
                                  
    def itemFits(self, item, x, y):
        if(x >= self.width or y >= self.height 
           or x + item.width > self.width or y + item.height > self.height):
            return False
                
        for y1 in range(y, y + item.height):
                for x1 in range(x, x + item.width):           
                    if(self.items[y1][x1] is not None):
                        return False
        return True
                       
    def addItemAt(self, item, x, y):
        item.x = x; item.y = y
        if(self.itemDict.has_key(item.id) or  not self.itemFits(item, x,y)):
            return None
        else:
            self.itemDict[item.id] = item
            self.__insertItem(item, x, y)
            return item                       
                            
    #Brute force search, inserting n 1x1 items is n^2 time
    def addItem(self, item):
        for y in range(0, self.height):
            for x in range(0, self.width):            
                if(self.itemFits(item, x, y)):
                    return self.addItemAt(item, x, y)

        return None
       
    def rmItem(self, item):
        if(item is None):
            return None
        
        if(self.itemDict.has_key(item.id)):
            self.itemDict.pop(item.id)
            for y in range(item.y, item.y + item.height):
                for x in range(item.x, item.x + item.width):                    
                    self.items[y][x] = None
            return item
        else:
            return None
        
    def rmItemAt(self, x, y):
        return self.rmItem(self.itemAt(x, y))
    
    def itemAt(self, x, y):
        if(x >= 0 and x < self.width and y >=0 and y < self.height):
            return self.items[y][x]
        else:
            return None

    def iterItems(self):
        return self.itemDict.itervalues()


def sortTest():
    #Make a bp with odd width. Fill it with 2x2 items, and a few single 1x2 items.
    #Make sure they all fit and sort works
    bp = Backpack(height=4, width = 9)
    for i in range(0, (4*9)/2):
            bp.addItem(Item("Item "+str(i), 2, 2))
    
    bp.addItem(Item("EdgeCase",1,2))
    bp.addItem(Item("EdgeCase2",1,2))
    bp.pack()
    for item in bp.iterItems(): print item

if __name__ == '__main__':
    bp = Backpack()
    bp.addItem(Item("Grandfather", 1, 3))
    bp.addItem(Item("Grand Vizeer's staff", 2, 3))
    bp.addItem(Item("The Executioner's Blade", 2, 3))
    for item in bp.iterItems(): print item
    bp.rmItem(bp.itemAt(0, 0))
    for item in bp.iterItems(): print item
    bp.addItemAt(Item("Foo's Blade", 2, 2), x=7, y=1)
    print bp.itemAt(7, 1)
    print "\n\n"
    sortTest()
    
    