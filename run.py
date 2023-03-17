from tkinter import *
import tkinter as tk
import tkinter.font as font
from tkinter import filedialog
import tkinter.ttk as ttk
from tkinter.ttk import *
import subprocess
from tkinter import messagebox
import os

# Create Object
root = Tk()

# Set geometry (widthxheight)
root.title("VCOM")
dir_path = os.path.dirname(os.path.realpath(__file__))
icon_path = os.path.join(dir_path, "archive.ico")

#root.geometry('420x150')
try:
	root.iconbitmap(icon_path)
except:
	pass
# This will create style object
style = Style()

# This will be adding style, and
# naming that style variable as
# W.Tbutton (TButton is used for ttk.Button).
style.configure('W.TButton', font=
('calibri', 10, 'bold', 'underline'),
				foreground='red')

# Style will be reflected only on
# this button because we are providing
# style only on this Button.
''' Button 1'''
btn1 = Button(root, text='Quit !',
			  style='W.TButton',
			  command=root.destroy)
# btn1.grid(row=0, column=3, padx=100)


''' Button 2'''

btn2 = Button(root, text='Click me !', command=None)
# btn2.grid(row=1, column=3, pady=10, padx=100)
style = ttk.Style()
style.configure(
        'Custom.TButton',
        background='#FFFFFF', # White
        bordercolor='#00FF00', # Green
        lightcolor='#FF0000', # Red
        darkcolor='#0000FF', # Blue
        borderwidth=4,
        foreground='#00FFFF', # Cyan
    )

def checkin():
	if filename == "":
		messagebox.showinfo("Information", "You should pick a file before any action")
		return
	res = subprocess.check_output(['./vcom', 'checkin', filename])
	res_split = res.decode().split("\n")
	for i in res_split:
		print(i)
	messagebox.showinfo("Information", res.decode())
		
	
def checkout():
	cli_cmds = ['./vcom', 'checkout']
	if filename == "":
		messagebox.showinfo("Information", "You should pick a file before any action")
		return
	if revision_num.get() != "":
		cli_cmds.append("-r")
		cli_cmds.append(str(revision_num.get()))
	if var.get():
		cli_cmds.append("-l")
		cli_cmds.append("lock")
	cli_cmds.append(filename)
	res = subprocess.check_output(cli_cmds)
	res_split = res.decode().split("\n")
	for i in res_split:
		print(i)
	messagebox.showinfo("Information", res.decode())

def diff():
	cli_cmds = ['./vcom', 'diff']
	if filename == "":
		messagebox.showinfo("Information", "You should pick a file before any action")
		return
	if revision_num.get() != "":
		cli_cmds.append("-r")
		cli_cmds.append(str(revision_num.get()))
	cli_cmds.append(filename)
	print(cli_cmds)
	res = subprocess.check_output(cli_cmds)
	res_split = res.decode().split("\n")[2:]
	def show_text(text):
		popup = tk.Toplevel()
		popup.title("Text")

		# Create a Text object and insert the specified text
		text_obj = tk.Text(popup)
		text_obj.insert(tk.END, text)
		text_obj.pack()
		text_obj.config(state="disabled")

		# Start the tkinter event loop
		popup.mainloop()
	has_changes = False
	for i in res_split:
		if i not in ["", "\n"]:
			has_changes = True
			break
	if not has_changes:
		messagebox.showinfo("Information", "There is no differences now")
		return
	res_split = "\n".join(res_split)
	show_text(res_split)

def login():
	def login_method():
		"""Function to get the login details when the login button is clicked"""
		username = username_entry.get()
		password = password_entry.get()
		res = subprocess.check_output(['./vcom', 'login', username, "-p", password])
		res_split = res.decode().split("\n")
		connected = False
		for i in res_split:
			if "connected" in i:
				connected = True
		if connected:
			messagebox.showinfo("Success", username + " logged in.")
		else:
			messagebox.showinfo("Error", "There is an error in the information.")
	
		popup.destroy()

	# Create a pop-up window
	popup = tk.Toplevel(root)
	popup.title("Login")
	popup.geometry("300x150")

	# Create frames for each field and center them
	username_frame = tk.Frame(popup)
	username_frame.grid(row=0, column=0, padx=20, pady=10)
	password_frame = tk.Frame(popup)
	password_frame.grid(row=1, column=0, padx=20, pady=10)
	button_frame = tk.Frame(popup)
	button_frame.grid(row=2, column=0, pady=10)

	# Create labels and entry fields for each frame
	username_label = tk.Label(username_frame, text="Username:")
	username_label.pack(side="left")
	username_entry = tk.Entry(username_frame)
	username_entry.pack(side="left")

	password_label = tk.Label(password_frame, text="Password:")
	password_label.pack(side="left")
	password_entry = tk.Entry(password_frame, show="*")
	password_entry.pack(side="left")
	
	# Create a login button and bind it to the login function
	login_button = tk.Button(button_frame, text="Login", command=login_method)
	login_button.pack()



frame0 = Frame(root)
frame0.grid(row=0, column=0, padx=5, pady=10)
btn_checkin = ttk.Button(frame0, text="checkin", width=12, command=checkin)
btn_checkout = Button(frame0, text="checkout", width=12, command=checkout)
btn_diff = Button(frame0, text="diff", width=12, command=diff)
btn_login = Button(frame0, text="login", width=12, command=login)

btn_checkin.grid(row=0, column=0, ipady=18)
btn_checkout.grid(row=0, column=1, ipady=18)
btn_diff.grid(row=0, column=2, ipady=18)
btn_login.grid(row=0, column=3, ipady=18)
filename = ""

def browse_button():
    global folder_path, filename
    filename = filedialog.askopenfile(mode="r").name
    folder_path.set(filename)
    print(filename)

# Create the dropdown menu
value = "Pick subject"
selected = tk.StringVar(value=value)

# path to exams
frame1 = Frame(root)
frame1.grid(row=1, column=0, padx=10, pady=10)
Label(frame1, text="Path to your file:").grid(row=0, column=0, sticky="w")

folder_path = StringVar()
Entry(master=frame1,textvariable=folder_path, width=58).grid(row=1, column=0)
tk.Button(frame1, text="Search", bg='royal blue', fg='white', command=browse_button).grid(row=1, column=2)

# path to exams
frame2 = Frame(root)
frame2.grid(row=2, column=0, padx=10, pady=10, sticky="w")
Label(frame2, text = "Revision (Last known if not specified):").grid(row=0, column=0,   columnspan=2, sticky="w")

def validate_input(x):
    """Function to validate the input of an Entry widget and only allow numbers"""
    if x.isdigit():
        return True
    elif x == "":
        return True
    else:
        return False


revision_num = StringVar()
validate_func = root.register(validate_input)
tk.Entry(frame2,textvariable=revision_num, validate="key", validatecommand=(validate_func, '%S'), width=28).grid(row=1, column=0)
var = tk.BooleanVar()
checkbox = tk.Checkbutton(frame2, text="lock", variable=var)
checkbox.grid(row=1, column=1)
tk.Button(frame2, text="Search", bg='royal blue', fg='white', command=browse_button)

# Execute Tkinter
root.mainloop()
