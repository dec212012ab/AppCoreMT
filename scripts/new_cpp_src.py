
import sys
import os
import PySimpleGUI as sg
import tkinter as tk
from tkinter import filedialog, messagebox
import re

root = tk.Tk()
root.withdraw()

sg.theme('DarkAmber')
window = None
layout = None

def create():
    global layout,window

    build_dir = os.path.abspath(os.path.dirname(sys.argv[0]))
    root_dir = os.path.abspath(os.path.dirname(build_dir))
    default_source_path = os.path.join(root_dir,'src')
    print("BUILDDIR:",build_dir)
    print("ROOTDIR:",root_dir)

    layout = [
        [sg.Text('CPP Boilerplate Generator')],
        [
            sg.Text("Source Directory:"),
            sg.InputText(default_text=os.path.join(os.path.dirname(sys.argv[0]),default_source_path),key='in_source_directory'),
            sg.Button('...',key='Browse')
        ],
        [
            sg.Text("Module Name:"), 
            sg.InputText(default_text="<Use format ModuleName>",key='in_module_name')
        ],
        [
            sg.Button('OK'),
            sg.Button('Cancel')
        ],
    ]

    window = sg.Window("CPP AUTOGEN",layout)

def run():
    global window
    if not window:
        print("ERROR: Window was not created!")
        exit(1)
    
    while True:
        event, values = window.read()

        if event == sg.WIN_CLOSED:
            break
        elif event == 'Cancel':
            break
        elif event == 'Browse':
            print("Browse clicked!")
            s = filedialog.askdirectory()
            if s != '':
                window['in_source_directory'].update(value=s)

        elif event == 'OK':
            if not os.path.exists(window['in_source_directory'].get()):
                messagebox.showinfo('Invalid Path',"The source directory path is invalid! ")
                continue
            if re.search("[^a-zA-Z0-9]",window['in_module_name'].get()):
                messagebox.showinfo('Invalid Module Name',"The module name must consist of only alphanumeric characters! (a-z,A-Z,0-9)")
                continue
            if not generateSource(window['in_source_directory'].get(),window['in_module_name'].get()):
                continue
            break

    window.close()
    pass

def generateSource(dest_dir,module_name):
    header_path = os.path.join(dest_dir,module_name+'.hpp')
    src_path = os.path.join(dest_dir,module_name+'.cpp')

    if os.path.isfile(header_path):
        messagebox.showerror("File Error","File " + header_path + " already exists!")
        return False

    if os.path.isfile(src_path):
        messagebox.showerror("File Error","File " + src_path + " already exists!")
        return False

    header = [
        '#ifndef __'+module_name.upper()+'_HPP__',
        '#define __'+module_name.upper()+'_HPP__',
        '\nclass '+module_name+'{',
        'public:',
        '\t'+module_name+'()=default;',
        '\t~'+module_name+'()=default;',
        'private:',
        '};',
        '#endif'
    ]

    src = [
        '#include \"'+module_name+'.hpp\"'
    ]

    with open(src_path,'w+') as f:
        f.writelines([s+'\n' for s in src])
    with open(header_path,'w+') as f:
        f.writelines([s+'\n' for s in header])
    
    return True

def main():
    create()
    run()


if __name__ == '__main__':
    main()
