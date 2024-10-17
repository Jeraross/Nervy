import tkinter as tk
import serial
import time
import json

# Inicializar conexão serial com o Arduino
arduino = serial.Serial('COM3', 9600)  # Verifique a porta COM do seu Arduino
time.sleep(2)  # Esperar a conexão estabilizar

# Leaderboard em formato de dicionário
leaderboard = []

# Função para carregar o leaderboard de um arquivo JSON
def carregar_leaderboard():
    global leaderboard
    try:
        with open('leaderboard.json', 'r') as f:
            leaderboard = json.load(f)
    except FileNotFoundError:
        leaderboard = []

# Função para salvar o leaderboard no arquivo JSON
def salvar_leaderboard():
    with open('leaderboard.json', 'w') as f:
        json.dump(leaderboard, f)

# Função para atualizar o leaderboard na GUI
def atualizar_leaderboard():
    leaderboard_display.delete(0, tk.END)
    for idx, player in enumerate(sorted(leaderboard, key=lambda x: x['score'], reverse=True)):
        leaderboard_display.insert(tk.END, f"{idx+1}. {player['name']} - {player['score']}")

# Função para iniciar o jogo no Arduino e receber a pontuação
def iniciar_jogo():
    player_name = player_name_entry.get()
    if player_name:
        arduino.write(b'INICIAR')  # Enviar comando para o Arduino começar o jogo
        time.sleep(10)  # Simular o tempo de espera para o jogo terminar

        # Ler a pontuação do Arduino
        score = int(arduino.readline().decode('utf-8').strip())
        leaderboard.append({'name': player_name, 'score': score})
        salvar_leaderboard()
        atualizar_leaderboard()

# Interface gráfica com tkinter
root = tk.Tk()
root.title("Leaderboard do Jogo Arduino")

# Exibir leaderboard
leaderboard_display = tk.Listbox(root, width=30, height=10)
leaderboard_display.pack()

# Input para nome do jogador
player_name_label = tk.Label(root, text="Nome do Jogador:")
player_name_label.pack()

player_name_entry = tk.Entry(root)
player_name_entry.pack()

# Botão para iniciar o jogo
start_button = tk.Button(root, text="Iniciar Jogo", command=iniciar_jogo)
start_button.pack()

# Carregar e atualizar leaderboard ao iniciar
carregar_leaderboard()
atualizar_leaderboard()

root.mainloop()

# Fechar a conexão serial ao encerrar
arduino.close()
