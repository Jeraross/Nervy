import serial
import time
import csv

# Configuração da porta serial
arduino_port = "COM10"  # Troque para a porta do seu Arduino
baud = 9600
ser = serial.Serial(arduino_port, baud)
time.sleep(2)

def receive_score_from_arduino():
    score = None
    
    while True:
        line = ser.readline().decode('utf-8').strip()
        
        # Verifica se é a linha de *score*
        if line.startswith("SCORE:"):
            score = int(line.split(":")[1])
            print(f"Pontuação recebida: {score}")
        
        # Verifica o sinal de fim
        if line == "END":
            break
    
    return score

def get_player_name():
    name = input("Digite o nome do jogador (3 letras): ").upper()[:3]
    return name

def save_to_leaderboard(name, score, filename="scores.csv"):
    with open(filename, mode="a", newline="") as file:
        writer = csv.writer(file)
        writer.writerow([name, score])
    print(f"{name} com {score} pontos foi adicionado ao scores.")

# Main loop que aguarda a pontuação do Arduino
try:
    print("Aguardando dados do Arduino...")
    while True:
        score = receive_score_from_arduino()
        
        if score is not None:
            player_name = get_player_name()
            save_to_leaderboard(player_name, score)
        time.sleep(1)
except KeyboardInterrupt:
    print("\nPrograma finalizado.")
finally:
    ser.close()
