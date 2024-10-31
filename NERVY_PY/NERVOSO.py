import serial
import time
import csv
import tkinter as tk
from tkinter import ttk, simpledialog, messagebox
import threading

# Configuração da porta serial e arquivo CSV
arduino_port = "COM10"  # Troque para a porta correta do seu Arduino
baud = 9600
ser = serial.Serial(arduino_port, baud)
csv_file = "scores.csv"
update_interval = 5000  # Intervalo de atualização em milissegundos
time.sleep(2)  # Tempo para estabelecer a conexão com a porta serial

# Função para receber pontuação do Arduino
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

# Função para salvar no placar
def save_to_leaderboard(name, score):
    with open(csv_file, mode="a", newline="") as file:
        writer = csv.writer(file)
        writer.writerow([name, score])
    print(f"{name} com {score} pontos foi adicionado ao placar.")

# Função para ler o placar do arquivo CSV
def read_leaderboard():
    scores = []
    try:
        with open(csv_file, newline='') as file:
            reader = csv.reader(file)
            for row in reader:
                try:
                    name = row[0]
                    score = int(row[1])
                    scores.append((name, score))
                except (ValueError, IndexError):
                    continue
        scores.sort(key=lambda x: x[1], reverse=True)
        return scores[:10]
    except FileNotFoundError:
        return []

# Função para atualizar o placar na interface
def update_leaderboard():
    scores = read_leaderboard()
    leaderboard_tree.delete(*leaderboard_tree.get_children())
    for name, score in scores:
        leaderboard_tree.insert("", "end", values=(name, score))

# Loop de atualização do placar
def refresh_loop():
    while True:
        update_leaderboard()
        time.sleep(update_interval / 1000)

# Função para solicitar o nome do jogador no thread principal
def ask_player_name(score):
    player_name = simpledialog.askstring("Nome do Jogador", "Digite o nome do jogador (3 letras):", parent=root)
    if player_name:
        player_name = player_name.upper()[:3]
        save_to_leaderboard(player_name, score)
    else:
        messagebox.showwarning("Aviso", "Nome do jogador não fornecido. A pontuação não será salva.")

# Loop principal que aguarda pontuações do Arduino
def receive_scores_loop():
    while True:
        score = receive_score_from_arduino()
        if score is not None:
            root.after(0, ask_player_name, score)

# Interface gráfica do leaderboard
root = tk.Tk()
root.title("Leaderboard")
root.geometry("400x500")
root.configure(bg="#1c1c1c")

frame = tk.Frame(root, bg="#2a2a2a", bd=10)
frame.pack(pady=20)

title = tk.Label(frame, text="NERVOSOS", font=("Press Start 2P", 24), bg="#2a2a2a", fg="#ffcc00")
title.pack(pady=10)

columns = ("Nome", "Pontuação")
leaderboard_tree = ttk.Treeview(frame, columns=columns, show="headings", height=10)

style = ttk.Style()
style.configure("Treeview",
                background="#2a2a2a",
                foreground="white",
                rowheight=30,
                fieldbackground="#2a2a2a")

leaderboard_tree.heading("Nome", text="Nome", anchor="center")
leaderboard_tree.heading("Pontuação", text="Pontuação", anchor="center")
leaderboard_tree.pack(fill="both", expand=True)

# Thread para atualização do placar
refresh_thread = threading.Thread(target=refresh_loop, daemon=True)
refresh_thread.start()

# Thread para receber pontuações do Arduino
receive_scores_thread = threading.Thread(target=receive_scores_loop, daemon=True)
receive_scores_thread.start()

root.mainloop()
