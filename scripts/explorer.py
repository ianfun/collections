# simple file and dir explorer
from tkinter import *
import os

root = Tk()
root.title(os.getcwd())

def open_dir(path):
    root.title(os.getcwd())
    os.chdir(path)
    global pane
    pane.destroy()
    pane = PanedWindow(root, orient=VERTICAL, width=1000, sashrelief=RAISED, sashwidth=2)
    dirs = os.listdir()
    newdir = ['..',]
    for i in dirs:
        if os.path.isdir(i):
            newdir.append(i)
    for i in dirs:
        if os.path.isfile(i):
            newdir.append(i)
    for i in newdir:
        if os.path.isfile(i):
            a = Label(pane, text=i, bg='#ABCDAB')
            pane.add(a)
        else:
            a = Button(pane, text=i, command=function(compile('open_dir(c)', '', 'exec'), {'open_dir': open_dir, 'c':i}))
            pane.add(a)

    pane.pack()

function = type(open_dir)
pane = PanedWindow()
open_dir(os.getcwd())
mainloop()
