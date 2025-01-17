import cv2
import face_recognition
import numpy as np
import requests
import serial
import json

with open("config.json", "r") as config_file:
    config = json.load(config_file)

# URL-ul fluxului ESP32-CAM
ESP32_URL = "http://192.168.84.77/capture"  # inlocuiește cu IP-ul ESP32-CAM

# Inițializare conexiune cu Arduino7                                                
arduino = serial.Serial('COM5', 115200, timeout=1)  # inlocuiește 'COM5' cu portul serial corespunzator

# incarca fetele cunoscute
known_faces = []
known_names = []
# Adauga fata cunoscuta "Gabi"
image_path_gabi = config["image_path_gabi"]
known_image_gabi = face_recognition.load_image_file(image_path_gabi)
known_face_encoding_gabi = face_recognition.face_encodings(known_image_gabi)[0]
known_faces.append(known_face_encoding_gabi)
known_names.append("Gabi")
# Adauga fata cunoscuta "Rares"
image_path_rares = config["image_path_rares"]
known_image_rares = face_recognition.load_image_file(image_path_rares)
known_face_encoding_rares = face_recognition.face_encodings(known_image_rares)[0]
known_faces.append(known_face_encoding_rares)
known_names.append("Rares")

print("Sistem pornit. Așteptăm cadre de la ESP32-CAM...")

# Loop principal pentru procesare cadre
while True:
    try:
        # Captureaza imaginea de la ESP32
        response = requests.get(ESP32_URL, timeout=5)
        if response.status_code != 200:
            print("Eroare la obținerea imaginii de la ESP32.")
            continue
    except requests.exceptions.RequestException as e:
        print(f"Eroare la conectarea la ESP32: {e}")
        continue
    # Decodifica imaginea
    img_array = np.array(bytearray(response.content), dtype=np.uint8)
    frame = cv2.imdecode(img_array, -1)
    if frame is None:
        print("Nu s-a putut decodifica imaginea.")
        continue
    # Conversie la RGB pentru face_recognition
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # Detectare fete si extragere encodari
    face_locations = face_recognition.face_locations(rgb_frame)
    face_encodings = face_recognition.face_encodings(rgb_frame, face_locations)
    face_recognized = False
    detected_name = "Necunoscut"
    for face_encoding in face_encodings:
        matches = face_recognition.compare_faces(known_faces, face_encoding, tolerance=0.6)
        if True in matches:
            match_index = matches.index(True)
            detected_name = known_names[match_index]
            face_recognized = True
            break
    # Trimitere rezultat la Arduino
    if face_recognized:
        print(f"Acces permis pentru utilizatorul: {detected_name}")
        try:
            arduino.write(b'PERMIS\n')  # Trimite PERMIS catre Arduino
        except serial.SerialException as e:
            print(f"Eroare la scrierea în Arduino: {e}")
    else:
        print("Acces respins.")
        try:
            arduino.write(b'RESPINS\n')  # Trimite RESPINS catre Arduino
        except serial.SerialException as e:
            print(f"Eroare la scrierea în Arduino: {e}")

    # Deseneaza rezultate pe imagine
    for (top, right, bottom, left) in face_locations:
        color = (0, 255, 0) if face_recognized else (0, 0, 255)
        message = f"Acces Permis: {detected_name}" if face_recognized else "Acces Respins"
        cv2.rectangle(frame, (left, top), (right, bottom), color, 2)
        cv2.putText(frame, message, (left, top - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, color, 2)

    cv2.imshow('ESP32-CAM', frame)

    # Apasa 'q' pentru a opri
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# inchidere conexiuni
cv2.destroyAllWindows()
arduino.close()