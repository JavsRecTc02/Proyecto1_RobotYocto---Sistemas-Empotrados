import tkinter as tk
import time

# Función para guardar los valores en el archivo de texto
def update_file(*args):
    try:
        with open("sim_data.txt", "w") as f:
            # Escribe: frontal izquierdo derecho
            f.write(f"{front_var.get():.1f} {left_var.get():.1f} {right_var.get():.1f}\n")
    except Exception as e:
        print(f"Error escribiendo archivo: {e}")

# Configuración de la ventana principal
root = tk.Tk()
root.title("Simulador de Sensores - Robot Yocto")
root.geometry("350x250")
root.configure(padx=20, pady=20)

# Variables (Inician en 50 cm, distancia segura)
front_var = tk.DoubleVar(value=50.0)
left_var = tk.DoubleVar(value=50.0)
right_var = tk.DoubleVar(value=50.0)

# Rastrear cambios en los sliders
front_var.trace_add("write", update_file)
left_var.trace_add("write", update_file)
right_var.trace_add("write", update_file)

tk.Label(root, text="Sensor Frontal (cm):", font=("Arial", 10, "bold")).pack(anchor="w")
tk.Scale(root, variable=front_var, from_=2.0, to=200.0, orient="horizontal", length=300).pack()

tk.Label(root, text="Sensor Izquierdo (cm):", font=("Arial", 10, "bold")).pack(anchor="w")
tk.Scale(root, variable=left_var, from_=2.0, to=200.0, orient="horizontal", length=300).pack()

tk.Label(root, text="Sensor Derecho (cm):", font=("Arial", 10, "bold")).pack(anchor="w")
tk.Scale(root, variable=right_var, from_=2.0, to=200.0, orient="horizontal", length=300).pack()

# Generar el archivo inicial
update_file()

tk.Label(root, text="Mueve los sliders para simular obstáculos", fg="gray").pack(pady=10)

root.mainloop()