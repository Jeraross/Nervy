import serial
import time
import csv
import tkinter as tk
from tkinter import simpledialog, messagebox
from PIL import Image, ImageTk  # Biblioteca para carregar a imagem PNG
import threading

# Configuração da porta serial e arquivo CSV
arduino_port = "COM7"  # Troque para a porta correta do seu Arduino
baud = 9600
ser = serial.Serial(arduino_port, baud)
csv_file = "scores.csv"
update_interval = 5000  # Intervalo de atualização em milissegundos
time.sleep(2)  # Tempo para estabelecer a conexão com a porta serial

# Variável global para o dia atual
dia = 1

# Funções de leaderboard e Arduino (não alteradas para essa modificação)
def receive_score_from_arduino():
    score = None
    while True:
        line = ser.readline().decode('utf-8').strip()
        if line.startswith("SCORE:"):
            score = int(line.split(":")[1])
            print(f"Pontuação recebida: {score}")
        if line == "END":
            break
    return score

def save_to_leaderboard(name, score, day):
    with open(csv_file, mode="a", newline="") as file:
        writer = csv.writer(file)
        writer.writerow([name, score, day])
    print(f"{name} com {score} pontos foi adicionado ao placar para o dia {day}.")

def read_leaderboard(day=None):
    scores_today = []
    scores_all_time = []
    try:
        with open(csv_file, newline='') as file:
            reader = csv.reader(file)
            for row in reader:
                try:
                    name = row[0]
                    score = int(row[1])
                    score_day = int(row[2])
                    if score_day == day:
                        scores_today.append((name, score))
                    scores_all_time.append((name, score))
                except (ValueError, IndexError):
                    continue
        scores_today.sort(key=lambda x: x[1], reverse=True)
        scores_all_time.sort(key=lambda x: x[1], reverse=True)
        return scores_today[:10], scores_all_time[:10]
    except FileNotFoundError:
        return [], []

def update_leaderboards():
    scores_today, scores_all_time = read_leaderboard(dia)
    for label, (name, score) in zip(today_name_labels, scores_today):
        label.config(text=name)
    for label, (name, score) in zip(today_score_labels, scores_today):
        label.config(text=score)
    for label in today_name_labels[len(scores_today):]:
        label.config(text="")
    for label in today_score_labels[len(scores_today):]:
        label.config(text="")
    for label, (name, score) in zip(all_time_name_labels, scores_all_time):
        label.config(text=name)
    for label, (name, score) in zip(all_time_score_labels, scores_all_time):
        label.config(text=score)
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
        player_name = player_name.upper()[:3]
        save_to_leaderboard(player_name, score, dia)
    else:
        messagebox.showwarning("Aviso", "Nome do jogador não fornecido. A pontuação não será salva.")

def receive_scores_loop():
    while True:
        score = receive_score_from_arduino()
        if score is not None:
            root.after(0, ask_player_name, score)

# Interface gráfica do leaderboard
root = tk.Tk()
root.title("Leaderboard")
root.attributes("-fullscreen", True)
root.configure(bg="#000230")

# Frame principal com os dois leaderboards
frame = tk.Frame(root, bg="#000230", bd=10)
frame.pack(expand=True)

# Carregar a imagem do título
image_path = "nervosinhos.png"  # Caminho para sua imagem PNG
title_image = Image.open(image_path)
title_image = title_image.resize((800, 200), Image.LANCZOS)  # Redimensiona a imagem conforme necessário
title_photo = ImageTk.PhotoImage(title_image)

# Label que exibe a imagem do título
title_label = tk.Label(frame, image=title_photo, bg="#000230")
title_label.image = title_photo  # Referência para evitar coleta de lixo da imagem
title_label.pack(pady=20)

# Configurações para as listas de leaderboard
today_name_labels, today_score_labels = [], []
all_time_name_labels, all_time_score_labels = [], []

# Leaderboard "TOP DIARIOS"
today_frame = tk.Frame(frame, bg="#000230")
today_frame.pack(side="left", padx=40)

today_label = tk.Label(today_frame, text="TOP DIARIOS", font=("Press Start 2P", 24), bg="#000230", fg="#fc4342")
today_label.grid(row=0, column=0, columnspan=2, pady=10)

for i in range(10):
    name_label = tk.Label(today_frame, text="", font=("Press Start 2P", 14), bg="#000230", fg="white", anchor="w")
    score_label = tk.Label(today_frame, text="", font=("Press Start 2P", 14), bg="#000230", fg="white", anchor="e")
    name_label.grid(row=i + 1, column=0, sticky="w", padx=(10, 5), pady=2)
    score_label.grid(row=i + 1, column=1, sticky="e", padx=(5, 10), pady=2)
    today_name_labels.append(name_label)
    today_score_labels.append(score_label)

# Leaderboard "TOP GLOBAIS"
all_time_frame = tk.Frame(frame, bg="#000230")
all_time_frame.pack(side="right", padx=40)

all_time_label = tk.Label(all_time_frame, text="TOP GLOBAIS", font=("Press Start 2P", 24), bg="#000230", fg="#f03211")
all_time_label.grid(row=0, column=0, columnspan=2, pady=10)

for i in range(10):
    name_label = tk.Label(all_time_frame, text="", font=("Press Start 2P", 14), bg="#000230", fg="white", anchor="w")
    score_label = tk.Label(all_time_frame, text="", font=("Press Start 2P", 14), bg="#000230", fg="white", anchor="e")
    name_label.grid(row=i + 1, column=0, sticky="w", padx=(10, 5), pady=2)
    score_label.grid(row=i + 1, column=1, sticky="e", padx=(5, 10), pady=2)
    all_time_name_labels.append(name_label)
    all_time_score_labels.append(score_label)

# Botão para sair do modo de tela cheia
exit_button = tk.Button(root, text="Sair", font=("Press Start 2P", 12), bg="#ff4040", fg="white", command=root.destroy)
exit_button.place(relx=0.98, rely=0.02, anchor="ne")

# Threads para atualizar leaderboard e receber pontuações
refresh_thread = threading.Thread(target=refresh_loop, daemon=True)
refresh_thread.start()

receive_scores_thread = threading.Thread(target=receive_scores_loop, daemon=True)
receive_scores_thread.start()

root.mainloop()
