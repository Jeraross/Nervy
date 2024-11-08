import serial, time, csv, tkinter as tk
from tkinter import simpledialog, messagebox
from PIL import Image, ImageTk
import threading

arduino_port = "COM7"
baud = 9600
ser = serial.Serial(arduino_port, baud)
csv_file = "scores.csv"
update_interval = 5000
time.sleep(2)
dia = 3

def receive_score_from_arduino():
    while True:
        line = ser.readline().decode('utf-8').strip()
        if line.startswith("SCORE:"):
            return int(line.split(":")[1])
        if line == "END":
            break

def save_to_leaderboard(name, score, day):
    with open(csv_file, mode="a", newline="") as file:
        writer = csv.writer(file)
        writer.writerow([name, score, day])

def read_leaderboard(day=None):
    scores_today, scores_all_time = [], []
    try:
        with open(csv_file, newline='') as file:
            reader = csv.reader(file)
            for row in reader:
                try:
                    name, score, score_day = row[0], int(row[1]), int(row[2])
                    if score_day == day:
                        scores_today.append((name, score))
                    scores_all_time.append((name, score))
                except:
                    continue
        scores_today.sort(key=lambda x: x[1], reverse=True)
        scores_all_time.sort(key=lambda x: x[1], reverse=True)
        return scores_today[:10], scores_all_time[:10]
    except FileNotFoundError:
        return [], []

def update_leaderboards():
    scores_today, scores_all_time = read_leaderboard(dia)
    for i, (name, score) in enumerate(scores_today):
        today_name_labels[i].config(text=f"{i+1}. {name}")
        today_score_labels[i].config(text=score)
    for label in today_name_labels[len(scores_today):]:
        label.config(text="")
    for label in today_score_labels[len(scores_today):]:
        label.config(text="")
    for i, (name, score) in enumerate(scores_all_time):
        all_time_name_labels[i].config(text=f"{i+1}. {name}")
        all_time_score_labels[i].config(text=score)
    for label in all_time_name_labels[len(scores_all_time):]:
        label.config(text="")
    for label in all_time_score_labels[len(scores_all_time):]:
        label.config(text="")

def refresh_loop():
    while True:
        update_leaderboards()
        time.sleep(update_interval / 1000)

def ask_player_name(score):
    player_name = simpledialog.askstring("Nome do Jogador", "Digite o nome do jogador (3 letras):", parent=root)
    if player_name:
        save_to_leaderboard(player_name.upper()[:3], score, dia)
    else:
        messagebox.showwarning("Aviso", "Nome do jogador não fornecido. A pontuação não será salva.")

def receive_scores_loop():
    while True:
        score = receive_score_from_arduino()
        scores_today, _ = read_leaderboard(dia)
        if score and (len(scores_today) < 10 or score > min(s[1] for s in scores_today)):
            root.after(0, ask_player_name, score)

root = tk.Tk()
root.title("Leaderboard")
root.attributes("-fullscreen", True)
root.configure(bg="#000230")

frame = tk.Frame(root, bg="#000230", bd=10)
frame.pack(expand=True)

image_path = "nervosinhos.png"
title_image = Image.open(image_path).resize((800, 200), Image.LANCZOS)
title_photo = ImageTk.PhotoImage(title_image)
title_label = tk.Label(frame, image=title_photo, bg="#000230")
title_label.image = title_photo
title_label.pack(pady=20)

today_name_labels, today_score_labels = [], []
all_time_name_labels, all_time_score_labels = [], []

today_frame = tk.Frame(frame, bg="#000230")
today_frame.pack(side="left", padx=40)
tk.Label(today_frame, text="TOP DIARIOS", font=("Press Start 2P", 24), bg="#000230", fg="#fc4342").grid(row=0, column=0, columnspan=3, pady=10)

for i in range(10):
    rank_label = tk.Label(today_frame, text=f"{i+1}.", font=("Press Start 2P", 14), bg="#000230", fg="white", anchor="w")
    name_label = tk.Label(today_frame, text="", font=("Press Start 2P", 14), bg="#000230", fg="white", anchor="w")
    score_label = tk.Label(today_frame, text="", font=("Press Start 2P", 14), bg="#000230", fg="white", anchor="e")
    rank_label.grid(row=i + 1, column=0, sticky="w", padx=(10, 5), pady=2)
    name_label.grid(row=i + 1, column=1, sticky="w", padx=(5, 5), pady=2)
    score_label.grid(row=i + 1, column=2, sticky="e", padx=(5, 10), pady=2)
    today_name_labels.append(name_label)
    today_score_labels.append(score_label)

all_time_frame = tk.Frame(frame, bg="#000230")
all_time_frame.pack(side="right", padx=40)
tk.Label(all_time_frame, text="TOP GLOBAIS", font=("Press Start 2P", 24), bg="#000230", fg="#f03211").grid(row=0, column=0, columnspan=3, pady=10)

for i in range(10):
    rank_label = tk.Label(all_time_frame, text=f"{i+1}.", font=("Press Start 2P", 14), bg="#000230", fg="white", anchor="w")
    name_label = tk.Label(all_time_frame, text="", font=("Press Start 2P", 14), bg="#000230", fg="white", anchor="w")
    score_label = tk.Label(all_time_frame, text="", font=("Press Start 2P", 14), bg="#000230", fg="white", anchor="e")
    rank_label.grid(row=i + 1, column=0, sticky="w", padx=(10, 5), pady=2)
    name_label.grid(row=i + 1, column=1, sticky="w", padx=(5, 5), pady=2)
    score_label.grid(row=i + 1, column=2, sticky="e", padx=(5, 10), pady=2)
    all_time_name_labels.append(name_label)
    all_time_score_labels.append(score_label)

exit_button = tk.Button(root, text="Sair", font=("Press Start 2P", 12), bg="#ff4040", fg="white", command=root.destroy)
exit_button.place(relx=0.98, rely=0.02, anchor="ne")

threading.Thread(target=refresh_loop, daemon=True).start()
threading.Thread(target=receive_scores_loop, daemon=True).start()
root.mainloop()
