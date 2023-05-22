import tkinter as tk
from tkinter import ttk
import json
import serial
import threading
import serial.tools.list_ports
import sys


class COMPortSelectionWindow:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("COM Port Selection")
        self.root.geometry("300x150")

        self.com_ports = []
        self.com_port_var = tk.StringVar(self.root)
        self.com_ports_dropdown = ttk.Combobox(self.root, textvariable=self.com_port_var)
        self.com_ports_dropdown.pack(pady=20)

        # Retrieve available COM ports
        ports = serial.tools.list_ports.comports()
        self.com_ports = [(port.device, port.manufacturer) for port in ports]

        # Set the default selection to the first port, if available
        if self.com_ports:
            self.com_port_var.set(self.com_ports[0][0])

        # Set the dropdown list values as "COM Port - Manufacturer"
        self.com_ports_dropdown['values'] = ["{} - {}".format(port[0], port[1]) for port in self.com_ports]

        self.start_button = ttk.Button(self.root, text="Start", command=self.start_robot_simulator)
        self.start_button.pack(pady=10)

        # Register the on_closing function as the callback when the window is closed
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.root.mainloop()

    def start_robot_simulator(self):
        selected_port = self.com_port_var.get().split(" - ")[0]  # Retrieve the selected COM port only
        self.root.destroy()
        robot = RobotSimulator(selected_port)
        robot.root.mainloop()

    def on_closing(self):
        self.root.destroy()
        sys.exit()


class RobotSimulator:
    def __init__(self, com_port):
        self.width = 500
        self.height = 350
        self.x = 50
        self.y = 300
        self.waypoint = None

        self.root = tk.Tk()
        self.root.title("Robot Simulator")
        self.canvas = tk.Canvas(self.root, width=self.width, height=self.height)
        self.canvas.pack(side="left")
        self.root.geometry("650x350")

        # Create the rectangular playing field
        self.canvas.create_rectangle(0, 0, self.width, self.height, fill="gray")

        # Create the distribution center
        self.canvas.create_rectangle(0, 150, 100, 250, fill="white")
        self.canvas.create_text(50, 200, text="Verteilzentrum", fill="black")

        # Create the start point
        self.canvas.create_rectangle(0, 250, 100, 350, fill="white")
        self.canvas.create_text(50, 300, text="Start", fill="black")

        # Create the end customers
        customer_width = 100
        customer_height = 100

        for i in range(3):
            x = 100 + customer_width * i
            y = 0

            self.canvas.create_rectangle(x, y, x + customer_width, y + customer_height, fill="white")
            self.canvas.create_text(x + customer_width / 2, y + customer_height / 2, text=f"Endkunde {i + 1}",
                                    fill="black")

        # Create end customer 4
        self.canvas.create_rectangle(self.width - 100, 150, self.width, 250, fill="white")
        self.canvas.create_text(self.width - 50, 200, text="Endkunde 4", fill="black")

        self.coordinates_frame = tk.Frame(self.root)
        self.coordinates_frame.pack(side="right", padx=10)

        self.x_label = tk.Label(self.coordinates_frame, text="X: 250")
        self.x_label.pack()

        self.y_label = tk.Label(self.coordinates_frame, text="Y: 400")
        self.y_label.pack()

        self.status_label = tk.Label(self.root, text="Finished")
        self.status_label.pack()

        self.serial_thread = threading.Thread(target=self.start_serial_communication, args=(com_port,))
        self.serial_thread.start()

        self.root.after(1000, self.update)  # Start the simulation loop

    def update(self):
        if self.waypoint:
            self.move_towards_waypoint()

        self.canvas.delete("robot")
        self.canvas.create_rectangle(self.x - 5, self.y - 5, self.x + 5, self.y + 5, fill="red", tags="robot")

        self.x_label.config(text="X: " + str(self.x))
        self.y_label.config(text="Y: " + str(self.y))

        self.status_label.config(text="Finished" if self.waypoint is None else "Busy")

        self.root.after(10, self.update)  # Update every 100 milliseconds

    def move_towards_waypoint(self):
        if self.x < self.waypoint['x']:
            self.x += 1
        elif self.x > self.waypoint['x']:
            self.x -= 1

        if self.y < self.waypoint['y']:
            self.y += 1
        elif self.y > self.waypoint['y']:
            self.y -= 1

        if self.x == self.waypoint['x'] and self.y == self.waypoint['y']:
            self.waypoint = None

    def set_next_waypoint(self, x, y):
        self.waypoint = {'x': x, 'y': y}

    def get_driving_state(self):
        state = "Busy" if self.waypoint is not None else "Finished"
        return {'Data': {'State': state}}

    def get_current_position(self):
        return {'Data': {'x': self.x, 'y': self.y}}

    def get_battery_state(self):
        return {'Data': {'BatteryState': '66'}}

    def process_command(self, command):
        if command['Command'] == 'SetNextWaypoint':
            x = int(command['Data']['x'])
            y = int(command['Data']['y'])
            self.set_next_waypoint(x, y)
        elif command['Command'] == 'GetDrivingState':
            state = self.get_driving_state()
            return json.dumps(state)
        elif command['Command'] == 'GetCurrentPosition':
            position = self.get_current_position()
            return json.dumps(position)
        elif command['Command'] == 'GetBatteryState':
            battery_state = self.get_battery_state()
            return json.dumps(battery_state)

    def start_serial_communication(self, port):
        ser = serial.Serial(port, baudrate=115200, timeout=1)

        while True:
            try:
                user_input = ser.readline().decode('utf-8').strip()
                command = json.loads(user_input)
                response = self.process_command(command)
                if response:
                    ser.write(response.encode('utf-8'))
            except ValueError:
                print("Invalid command format. Please enter a valid JSON command.")


com_port_window = COMPortSelectionWindow()
